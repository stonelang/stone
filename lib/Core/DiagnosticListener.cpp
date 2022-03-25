#include "stone/Core/DiagnosticListener.h"

using stone::DiagnosticListener;

DiagnosticListener::~DiagnosticListener() {}

bool DiagnosticListener::IncludeInDiagnosticCounts() const {}

void DiagnosticListener::OnDiagnostic(const EmissionDiagnostic &diagnostic) {

  printf("%s", "DiagnosticListener::Listen\n");
}

/// Callback to inform the diagnostic client that processing of all
/// source files has ended.
void DiagnosticListener::Finish() {}

void DiagnosticListener::Flush() {}
