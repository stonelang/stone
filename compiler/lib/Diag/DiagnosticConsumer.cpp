#include "stone/Diag/DiagnosticConsumer.h"

using stone::DiagnosticConsumer;

DiagnosticConsumer::DiagnosticConsumer(DiagnosticEmitter &emitter)
    : emitter(emitter) {}

DiagnosticConsumer::~DiagnosticConsumer() {}

bool DiagnosticConsumer::IncludeInDiagnosticCounts() const {}

void DiagnosticConsumer::Consume(const DiagnosticMessage &diagnostic) {
  // TODO: Diag stuff there like warnnings and errors
}

void DiagnosticConsumer::Consume(const DiagnosticEmitter &emitter,
                                 const DiagnosticMessage &diagnostic) {
  // TODO: Diag stuff there like warnnings and errors
}
/// Callback to inform the diagnostic client that processing of all
/// source files has ended.
bool DiagnosticConsumer::Finish() { return false; }
