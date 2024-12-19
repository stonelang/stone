#include "stone/Diag/TextDiagnosticBuffering.h"
#include "stone/Diag/DiagnosticEngine.h"

using namespace stone;

diags::TextDiagnosticBuffering::TextDiagnosticBuffering() {}

void diags::TextDiagnosticBuffering::HandleDiagnostic(DiagnosticLevel Level,
                                            const DiagnosticInfo &Info) {
  // Default implementation (Warnings/errors count).
  DiagnosticClient::HandleDiagnostic(Level, Info);
}

void diags::TextDiagnosticBuffering::FlushDiagnostics(
    DiagnosticEngine &Diags) const {
  // for (const auto &I : All) {
  //   auto Diag = Diags.Report(Diags.getCustomDiagID(I.first, "%0"));
  //   switch (I.first) {
  //   default: llvm_unreachable(
  //                          "Diagnostic not handled during diagnostic
  //                          flushing!");
  //   case DiagnosticsEngine::Note:
  //     Diag << Notes[I.second].second;
  //     break;
  //   case DiagnosticsEngine::Warning:
  //     Diag << Warnings[I.second].second;
  //     break;
  //   case DiagnosticsEngine::Remark:
  //     Diag << Remarks[I.second].second;
  //     break;
  //   case DiagnosticsEngine::Error:
  //   case DiagnosticsEngine::Fatal:
  //     Diag << Errors[I.second].second;
  //     break;
  //   }
  // }
}
