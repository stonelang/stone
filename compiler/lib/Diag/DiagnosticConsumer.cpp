#include "stone/Diag/DiagnosticConsumer.h"

using stone::DiagnosticConsumer;

DiagnosticConsumer::DiagnosticConsumer(DiagnosticEmitter &emitter)
    : emitter(emitter) {}

DiagnosticConsumer::~DiagnosticConsumer() {}

bool DiagnosticConsumer::IncludeInDiagnosticCounts() const {}

/// Callback to inform the diagnostic client that processing of all
/// source files has ended.
bool DiagnosticConsumer::Finish() { return false; }
