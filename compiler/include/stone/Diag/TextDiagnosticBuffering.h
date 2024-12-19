#ifndef STONE_DIAG_TEXT_DIAGNOSTIC_BUFFERING_H
#define STONE_DIAG_TEXT_DIAGNOSTIC_BUFFERING_H

#include "stone/Diag/DiagnosticClient.h"

namespace stone {

namespace diags {
class DiagnosticEngine;
class TextDiagnosticBuffering final : public DiagnosticClient {

public:
  TextDiagnosticBuffering();

public:
  void HandleDiagnostic(DiagnosticLevel DiagLevel,
                        const DiagnosticInfo &Info) override;

  /// FlushDiagnostics - Flush the buffered diagnostics to an given
  /// diagnostic engine.
  void FlushDiagnostics(DiagnosticEngine &Diags) const;
};

} // namespace diags

} // namespace stone

#endif