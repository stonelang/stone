#include "stone/Diag/DiagnosticClient.h"

using namespace stone;

diags::DiagnosticClient::DiagnosticClient(Ownership ownership)
    : ownership(ownership) {}

diags::DiagnosticClient::~DiagnosticClient() = default;

/// IncludeInDiagnosticCounts - This method (whose default implementation
///  returns true) indicates whether the diagnostics handled by this
///  DiagnosticConsumer should be included in the number of diagnostics
///  reported by DiagnosticsEngine.
bool diags::DiagnosticClient::UseInDiagnosticCounts() const { return true; }

void diags::DiagnosticClient::HandleDiagnostic(DiagnosticLevel DiagLevel,
                                               const DiagnosticInfo &Info) {
  if (!UseInDiagnosticCounts()) {
    return;
  }

  // if (DiagLevel == DiagnosticsEngine::Warning)
  //   ++NumWarnings;
  // else if (DiagLevel >= DiagnosticsEngine::Error)
  //   ++NumErrors;
}
