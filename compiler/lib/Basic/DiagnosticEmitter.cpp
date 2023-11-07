#include "stone/Basic/DiagnosticEmitter.h"
#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticListener;

DiagnosticEmitter::DiagnosticEmitter(DiagnosticFormatter &formatter)
    : formatter(formatter) {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}