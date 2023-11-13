#include "stone/Diag/DiagnosticEmitter.h"
#include "stone/Diag/DiagnosticConsumer.h"

using stone::DiagnosticConsumer;
using stone::DiagnosticEmitter;

DiagnosticEmitter::DiagnosticEmitter(DiagnosticFormatter &formatter)
    : formatter(formatter) {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}