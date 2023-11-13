#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/CoreDiagnostic.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

using namespace stone;

using stone::DiagnosticEngine;
using stone::InFlightDiagnostic;

namespace {
enum class LocalOptions {
  /// No options.
  none,

  /// The location of this diagnostic points to the beginning of the first
  /// token that the parser considers invalid.  If this token is located at the
  /// beginning of the line, then the location is adjusted to point to the end
  /// of the previous token.
  ///
  /// This behavior improves experience for "expected token X" diagnostics.
  PointsToFirstBadToken,

  /// After a fatal error subsequent diagnostics are suppressed.
  Fatal,
};

struct LocalDiagnostic {
  diag::Level severity : 2;
  bool pointsToFirstBadToken : 1;
  bool isFatal : 1;

  constexpr LocalDiagnostic(diag::Level severity, bool firstBadToken,
                            bool fatal)

      : severity(severity), pointsToFirstBadToken(firstBadToken),
        isFatal(fatal) {}
  constexpr LocalDiagnostic(diag::Level severity, LocalOptions opts)
      : LocalDiagnostic(severity, opts == LocalOptions::PointsToFirstBadToken,
                        opts == LocalOptions::Fatal) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Diag/DiagnosticEngine.def"
  MAX
};
} // end anonymous namespace

// TODO: categorization
static const constexpr LocalDiagnostic LocalDiagnostics[] = {
#define ERROR(ID, Options, Text, Signature)                                    \
  LocalDiagnostic(diag::Level::Error, LocalOptions::Options),
#define WARN(ID, Options, Text, Signature)                                     \
  LocalDiagnostic(diag::Level::Warn, LocalOptions::Options),
#define NOTE(ID, Options, Text, Signature)                                     \
  LocalDiagnostic(diag::Level::Note, LocalOptions::Options),
#define REMARK(ID, Options, Text, Signature)                                   \
  LocalDiagnostic(dia::Level::Remark, LocalOptions::Options),
#include "stone/Diag/DiagnosticEngine.def"
};

static_assert((sizeof(LocalDiagnostics) / sizeof(LocalDiagnostic)) ==
                  LocalDiagID::MAX,
              "array size mismatch");

static constexpr const char *const DiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text,
#include "stone/Diag/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const DebugDiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text " [" #ID "]",
#include "stone/Diag/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const DiagnosticIDStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) #ID,
#include "stone/Diag/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const CodeFixStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIX(ID, Text, Signature) Text,
#include "stone/Diag/DiagnosticEngine.def"
    "<not a fix-it>",
};

static diag::Level ToDiagnosticLevel(diag::Level severity, bool isFatal) {
  switch (severity) {
  case diag::Level::Note:
    return diag::Level::Note;
  case diag::Level::Error:
    return isFatal ? diag::Level::Fatal : diag::Level::Error;
  case diag::Level::Warn:
    return diag::Level::Warn;
  case diag::Level::Remark:
    return diag::Level::Remark;
  }
  llvm_unreachable("Unhandled diagnostic severity in switch.");
}
// A special option only for compiler writers that causes Diagnostics to assert
// when a failure diagnostic is emitted. Intended for use in the debugger.
llvm::cl::opt<bool> AssertOnError("stone-diagnostics-assert-on-error",
                                  llvm::cl::init(false));
// A special option only for compiler writers that causes Diagnostics to assert
// when a warning diagnostic is emitted. Intended for use in the debugger.
llvm::cl::opt<bool> AssertOnWarn("stone-diagnostics-assert-on-warning",
                                 llvm::cl::init(false));

DiagnosticState::DiagnosticState() {
  // Initialize our ignored diagnostics to default
  ignoredDiagnostics.resize(LocalDiagID::MAX);
}

diag::Level DiagnosticState::DetermineLevel(const Diagnostic &diag) {
  // We determine how to handle a diagnostic based on the following rules
  //   1) Map the diagnostic to its "intended" behavior, applying the behavior
  //      limit for this particular emission
  //   2) If current state dictates a certain behavior, follow that
  //   3) If the user ignored this specific diagnostic, follow that
  //   4) If the user substituted a different behavior for this behavior, apply
  //      that change
  //   5) Update current state for use during the next diagnostic

  //   1) Map the diagnostic to its "intended" behavior, applying the behavior
  //      limit for this particular emission
  auto diagnostic = LocalDiagnostics[(unsigned)diag.GetID()];
  diag::Level lvl =
      std::max(ToDiagnosticLevel(diagnostic.severity, diagnostic.isFatal),
               diag.GetLevelLimit());
  assert(lvl != diag::Level::None);

  //   2) If current state dictates a certain behavior, follow that

  // Notes relating to ignored diagnostics should also be ignored
  if (prevLevel == diag::Level::Ignore && lvl == diag::Level::Note)
    lvl = diag::Level::Ignore;

  // Suppress diagnostics when in a fatal state, except for follow-on notes
  if (fatalErrorOccurred)
    if (!showDiagnosticsAfterFatalError && lvl != diag::Level::Note)
      lvl = diag::Level::Ignore;

  //   3) If the user ignored this specific diagnostic, follow that
  if (ignoredDiagnostics[(unsigned)diag.GetID()])
    lvl = diag::Level::Ignore;

  //   4) If the user substituted a different behavior for this behavior, apply
  //      that change
  if (lvl == diag::Level::Warn) {
    if (warningsAsErrors)
      lvl = diag::Level::Error;
    if (suppressWarnings)
      lvl = diag::Level::Ignore;
  }

  //   5) Update current state for use during the next diagnostic
  if (lvl == diag::Level::Fatal) {
    fatalErrorOccurred = true;
    anyErrorOccurred = true;
  } else if (lvl == diag::Level::Error) {
    anyErrorOccurred = true;
  }

  assert((!AssertOnError || !anyErrorOccurred) && "We emitted an error?!");
  assert((!AssertOnWarn || (lvl != diag::Level::Warn)) &&
         "We emitted a warning?!");

  prevLevel = lvl;
  return lvl;
}

InFlightDiagnostic::InFlightDiagnostic()
    : de(0), tokenable(0), isActive(true), fixer(*this) {}

InFlightDiagnostic::InFlightDiagnostic(DiagnosticEngine &de,
                                       Tokenable *tokenable)
    : de(&de), fixer(*this), isActive(true), isForceFlush(false),
      tokenable(tokenable) {}

DiagnosticEngine::DiagnosticEngine(SrcMgr &sm) : sm(sm), curDiagnostic() {}

// TODO:
//  diag::Level DiagnosticEngine::GetSeverityByDiagID(const DiagID id) {
//    return LocalDiagnostics[(unsigned)id].severity;
//  }

llvm::StringRef DiagnosticEngine::GetDiagString(const DiagID diagID,
                                                bool printDiagnosticName) {
  // TODO: Localization
  if (printDiagnosticName) {
    return DebugDiagnosticStrings[(unsigned)diagID];
  }
  return DiagnosticStrings[(unsigned)diagID];
}

llvm::StringRef DiagnosticEngine::GetDiagIDStringByDiagID(const DiagID diagID) {
  return DiagnosticIDStrings[(unsigned)diagID];
}

void InFlightDiagnostic::Flush() {
  // If this diagnostic is inactive, then its soul was stolen by the copy ctor
  // (or by a subclass, as in SemaInFlightDiagnostic).
  if (IsActive()) {
    // de.GetCurrentDiagnostic().GetLangContext().Flush();
    if (de) {
      de->FlushCurrentDiagnostic();
    }
    // Clear();
  }
}

void DiagnosticEngine::FlushCurrentDiagnostic() {
  assert(curDiagnostic && "No active diagnostic to flush");
  EmitCurrentDiagnostic(true);
  curDiagnostic.reset();
}

bool DiagnosticEngine::HasError() { return false; }

void DiagnosticEngine::Print(ColorStream &os,
                             const PrintingPolicy *policy) const {}

void DiagnosticEngine::EmitDiagnostic(const Diagnostic &diagnostic) {

  auto diagnosticEvent = CreateDiagnosticMessage(diagnostic);
  for (auto &listener : listeners) {
    listener->Listen(*diagnosticEvent);
  }
}

void DiagnosticEngine::EmitPendingDiagnostics() {}

bool DiagnosticEngine::EmitCurrentDiagnostic(bool force) {
  EmitDiagnostic(*curDiagnostic);
}

llvm::Optional<DiagnosticMessage>
DiagnosticEngine::CreateDiagnosticMessage(const Diagnostic &diagnostic) {
  return DiagnosticMessage(
      /*TODO*/ diag::Level::Warn, diagnostic, GetSrcMgr(),
      GetDiagString(diagnostic.GetID(), true),
      /*TODO*/ llvm::StringRef());
}

bool DiagnosticEngine::Finish() {
  bool hadError = false;
  for (auto &listener : listeners) {
    hadError |= listener->Finish();
  }
  return hadError;
}
