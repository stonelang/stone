#include "stone/Basic/DiagnosticEmitter.h"
#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticListener;

DiagnosticEmitter::DiagnosticEmitter() {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticEvent &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}