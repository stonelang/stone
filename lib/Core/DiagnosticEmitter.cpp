#include "stone/Core/DiagnosticEmitter.h"
#include "stone/Core/DiagnosticListener.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticListener;

DiagnosticEmitter::DiagnosticEmitter() {}

DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const EmissionDiagnostic &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}