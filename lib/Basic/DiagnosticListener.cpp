#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticListener;

DiagnosticListener::~DiagnosticListener() {}

bool DiagnosticListener::IncludeInDiagnosticCounts() const {}

void DiagnosticListener::Listen(diag::Level level,
                                const EmissionDiagnostic &diagnostic) {

  printf("%s", "print basic stuff\n");
}