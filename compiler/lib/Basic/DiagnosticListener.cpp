#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticListener;

DiagnosticListener::DiagnosticListener(DiagnosticEmitter &emitter)
    : emitter(emitter) {}

DiagnosticListener::~DiagnosticListener() {}

bool DiagnosticListener::IncludeInDiagnosticCounts() const {}

void DiagnosticListener::OnDiagnostic(const DiagnosticMessage &diagnostic) {
  // TODO: Diag stuff there like warnnings and errors
}

/// Callback to inform the diagnostic client that processing of all
/// source files has ended.
bool DiagnosticListener::Finish() { return false; }
