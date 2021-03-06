#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticListener;

DiagnosticListener::~DiagnosticListener() {}

bool DiagnosticListener::IncludeInDiagnosticCounts() const {}

void DiagnosticListener::Listen(diag::Level level,
                                const EmissionDiagnostic &diagnostic) {

  printf("%s", "print basic stuff\n");
}

/// Callback to inform the diagnostic client that processing of all
/// source files has ended.
void DiagnosticListener::Finish() {}

void DiagnosticListener::Flush() {}
