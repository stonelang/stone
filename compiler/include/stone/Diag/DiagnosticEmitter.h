#ifndef STONE_DIAG_DIAGNOSTIC_EMITTER_H
#define STONE_DIAG_DIAGNOSTIC_EMITTER_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticClient.h"

namespace stone {
class SrcLoc;
class LangOptions;
class DiagnosticOptions;

namespace diags {

/// DiagnosticRenderer in clang
class DiagnosticEmitter {
protected:
  const LangOptions &LangOpts;

  /// The location of the previous diagnostic if known.
  ///
  /// This will be invalid in cases where there is no (known) previous
  /// diagnostic location, or that location itself is invalid or comes from
  /// a different source manager than SM.
  SrcLoc LastLoc;

  /// The level of the last diagnostic emitted.
  ///
  /// The level of the last diagnostic emitted. Used to detect level changes
  /// which change the amount of information displayed.
  DiagnosticLevel LastLevel = DiagnosticLevel::Ignored;

  DiagnosticEmitter(const LangOptions &LangOpts);

  virtual ~DiagnosticEmitter();
};

} // namespace diags

} // namespace stone

#endif