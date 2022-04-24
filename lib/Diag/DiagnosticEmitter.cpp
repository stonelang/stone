#include "stone/Diag/DiagnosticEmitter.h"
#include "stone/Diag/DiagnosticListener.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticListener;

DiagnosticEmitter::DiagnosticEmitter() {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticEvent &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}