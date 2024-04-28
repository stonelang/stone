#include "stone/Support/DiagnosticEngine.h"
#include "stone/Support/DiagnosticsCore.h"
#include "stone/Support/LexerBase.h"

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
  DiagnosticLevel severity : 2;
  bool pointsToFirstBadToken : 1;
  bool isFatal : 1;

  constexpr LocalDiagnostic(DiagnosticLevel severity, bool firstBadToken,
                            bool fatal)

      : severity(severity), pointsToFirstBadToken(firstBadToken),
        isFatal(fatal) {}
  constexpr LocalDiagnostic(DiagnosticLevel severity, LocalOptions opts)
      : LocalDiagnostic(severity, opts == LocalOptions::PointsToFirstBadToken,
                        opts == LocalOptions::Fatal) {}
};

// Reproduce the DiagIDs, as we want both the size and access to the raw ids
// themselves.
enum LocalDiagID : uint32_t {
#define DIAG(KIND, ID, Options, Text, Signature) ID,
#include "stone/Support/DiagnosticEngine.def"
  MAX
};
} // end anonymous namespace

// TODO: categorization
static const constexpr LocalDiagnostic LocalDiagnostics[] = {
#define ERROR(ID, Options, Text, Signature)                                    \
  LocalDiagnostic(DiagnosticLevel::Error, LocalOptions::Options),
#define WARN(ID, Options, Text, Signature)                                     \
  LocalDiagnostic(DiagnosticLevel::Warn, LocalOptions::Options),
#define NOTE(ID, Options, Text, Signature)                                     \
  LocalDiagnostic(DiagnosticLevel::Note, LocalOptions::Options),
#define REMARK(ID, Options, Text, Signature)                                   \
  LocalDiagnostic(dia::Level::Remark, LocalOptions::Options),
#include "stone/Support/DiagnosticEngine.def"
};

static_assert((sizeof(LocalDiagnostics) / sizeof(LocalDiagnostic)) ==
                  LocalDiagID::MAX,
              "array size mismatch");

static constexpr const char *const DiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text,
#include "stone/Support/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const DebugDiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text " [" #ID "]",
#include "stone/Support/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const DiagnosticIDStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) #ID,
#include "stone/Support/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const DiagnosticFixStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIX(ID, Text, Signature) Text,
#include "stone/Support/DiagnosticEngine.def"
    "<not a fix-it>",
};

static DiagnosticLevel ToDiagnosticLevel(DiagnosticLevel severity,
                                         bool isFatal) {
  switch (severity) {
  case DiagnosticLevel::Note:
    return DiagnosticLevel::Note;
  case DiagnosticLevel::Error:
    return isFatal ? DiagnosticLevel::Fatal : DiagnosticLevel::Error;
  case DiagnosticLevel::Warn:
    return DiagnosticLevel::Warn;
  case DiagnosticLevel::Remark:
    return DiagnosticLevel::Remark;
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

DiagnosticLevel DiagnosticState::DetermineLevel(const Diagnostic &diag) {
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
  DiagnosticLevel lvl =
      std::max(ToDiagnosticLevel(diagnostic.severity, diagnostic.isFatal),
               diag.GetLevelLimit());
  assert(lvl != DiagnosticLevel::None);

  //   2) If current state dictates a certain behavior, follow that

  // Notes relating to ignored diagnostics should also be ignored
  if (prevLevel == DiagnosticLevel::Ignore && lvl == DiagnosticLevel::Note)
    lvl = DiagnosticLevel::Ignore;

  // Suppress diagnostics when in a fatal state, except for follow-on notes
  if (fatalErrorOccurred)
    if (!showDiagnosticsAfterFatalError && lvl != DiagnosticLevel::Note)
      lvl = DiagnosticLevel::Ignore;

  //   3) If the user ignored this specific diagnostic, follow that
  if (ignoredDiagnostics[(unsigned)diag.GetID()])
    lvl = DiagnosticLevel::Ignore;

  //   4) If the user substituted a different behavior for this behavior, apply
  //      that change
  if (lvl == DiagnosticLevel::Warn) {
    if (warningsAsErrors)
      lvl = DiagnosticLevel::Error;
    if (suppressWarnings)
      lvl = DiagnosticLevel::Ignore;
  }

  //   5) Update current state for use during the next diagnostic
  if (lvl == DiagnosticLevel::Fatal) {
    fatalErrorOccurred = true;
    anyErrorOccurred = true;
  } else if (lvl == DiagnosticLevel::Error) {
    anyErrorOccurred = true;
  }

  assert((!AssertOnError || !anyErrorOccurred) && "We emitted an error?!");
  assert((!AssertOnWarn || (lvl != DiagnosticLevel::Warn)) &&
         "We emitted a warning?!");

  prevLevel = lvl;
  return lvl;
}

InFlightDiagnostic::InFlightDiagnostic() : de(0), isActive(true) {}

InFlightDiagnostic::InFlightDiagnostic(DiagnosticEngine &de)
    : de(&de), isActive(true), isForceFlush(false) {}

DiagnosticEngine::DiagnosticEngine(SrcMgr &sm) : sm(sm), curDiagnostic() {}

// TODO:
//  DiagnosticLevel DiagnosticEngine::GetSeverityByDiagID(const DiagID id) {
//    return LocalDiagnostics[(unsigned)id].severity;
//  }

llvm::StringRef DiagnosticEngine::GetDiagIDString(const DiagID diagID,
                                                bool printDiagnosticName) {
  // TODO: Localization
  if (printDiagnosticName) {
    return DebugDiagnosticStrings[(unsigned)diagID];
  }
  return DiagnosticStrings[(unsigned)diagID];
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
  for (auto &consumer : consumers) {
    consumer->ConsumeDiagnostic(*diagnosticEvent);
  }
}

void DiagnosticEngine::EmitPendingDiagnostics() {}

bool DiagnosticEngine::EmitCurrentDiagnostic(bool force) {
  EmitDiagnostic(*curDiagnostic);
}

std::optional<DiagnosticMessage>
DiagnosticEngine::CreateDiagnosticMessage(const Diagnostic &diagnostic) {
  return DiagnosticMessage(
      /*TODO*/ DiagnosticLevel::Warn, diagnostic, GetSrcMgr(),
      GetDiagIDString(diagnostic.GetID(), true),
      /*TODO*/ llvm::StringRef());
}

bool DiagnosticEngine::FinishProcessing() {
  bool hadError = false;
  for (auto &consumer : consumers) {
    hadError |= consumer->FinishProcessing();
  }
  return hadError;
}
