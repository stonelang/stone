#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticListener;

DiagnosticListener::DiagnosticListener() : colorfulStream() {}

DiagnosticListener::~DiagnosticListener() {}

bool DiagnosticListener::IncludeInDiagnosticCounts() const {}

void DiagnosticListener::OnDiagnostic(const DiagnosticEvent &diagnostic) {
  // TODO: Basic stuff there like warnnings and errors
}

/// Callback to inform the diagnostic client that processing of all
/// source files has ended.
void DiagnosticListener::Finish() {}

void DiagnosticListener::Flush() {}
