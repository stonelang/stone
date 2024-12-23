#ifndef STONE_DIAG_TEXT_DIAGNOSTIC_BUFFERING_H
#define STONE_DIAG_TEXT_DIAGNOSTIC_BUFFERING_H

#include "stone/Diag/DiagnosticClient.h"

namespace stone {

class DiagnosticEngine;

// // THINK: This may not be needed
// class TextDiagnosticBuffering final : public DiagnosticClient {

// public:
//   TextDiagnosticBuffering();

// public:
//   void HandleDiagnostic(const DiagnosticImpl &DI) override;

//   /// FlushDiagnostics - Flush the buffered diagnostics to an given
//   /// diagnostic engine.
//   void FlushDiagnostics(DiagnosticEngine &Diags) const;
// };

} // namespace stone

#endif