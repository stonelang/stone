#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/CoreDiagnostic.h"

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
#include "stone/Basic/DiagnosticEngine.def"
  Last
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
#include "stone/Basic/DiagnosticEngine.def"
};

static_assert((sizeof(LocalDiagnostics) / sizeof(LocalDiagnostic)) ==
                  LocalDiagID::Last,
              "array size mismatch");

static constexpr const char *const DiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text,
#include "stone/Basic/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const DebugDiagnosticStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature) Text " [" #ID "]",
#include "stone/Basic/DiagnosticEngine.def"
    "<not a diagnostic>",
};

static constexpr const char *const FixItStrings[] = {
#define DIAG(KIND, ID, Options, Text, Signature)
#define FIX(ID, Text, Signature) Text,
#include "stone/Basic/DiagnosticEngine.def"
    "<not a fix-it>",
};

void InFlightDiagnostic::Flush() {
  // If this diagnostic is inactive, then its soul was stolen by the copy ctor
  // (or by a subclass, as in SemaInFlightDiagnostic).
  if (IsActive()) {
    // de.GetCurrentDiagnostic().GetContext().Flush();
    de.FlushCurrentDiagnostic();

    // Clear();
  }
}

DiagnosticEngine::DiagnosticEngine(DiagnosticOptions &diagOpts, SrcMgr *sm)
    : diagOpts(diagOpts), sm(sm) {}

llvm::StringRef DiagnosticEngine::GetDiagString(const DiagID diagID,
                                                bool printDiagnosticName) {
  // TODO: Localization
  if (printDiagnosticName) {
    return DebugDiagnosticStrings[(unsigned)diagID];
  }
  return DiagnosticStrings[(unsigned)diagID];
}

void DiagnosticEngine::FlushCurrentDiagnostic() {

  assert(curDiagnostic && "No active diagnostic to flush");
  EmitCurrentDiagnostic(true);
  curDiagnostic.reset();
}

bool DiagnosticEngine::HasError() { return false; }

void DiagnosticEngine::Print(ColorOutputStream &os,
                             const PrintingPolicy *policy) const {}

void DiagnosticEngine::EmitSpecificDiagnostic(const Diagnostic &diagnostic) {

  auto emissionDiagnostic = BuildEmissionDiagnostic(diagnostic);
  for (auto &listener : listeners) {
    listener->Listen(/*TODO*/ diag::Level::Warn, *emissionDiagnostic);
  }
}

void DiagnosticEngine::EmitPendingDiagnostics() {}

bool DiagnosticEngine::EmitCurrentDiagnostic(bool force) {

  EmitSpecificDiagnostic(*curDiagnostic);
}

llvm::Optional<EmissionDiagnostic>
DiagnosticEngine::BuildEmissionDiagnostic(const Diagnostic &diagnostic) {

  return EmissionDiagnostic(
      diagnostic, GetDiagString(diagnostic.GetContext().GetDiagID(), true),
      /*TODO*/ llvm::StringRef());
}

void DiagnosticEngine::Finish() { FlushListeners(); }
