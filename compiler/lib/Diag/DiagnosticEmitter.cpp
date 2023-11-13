#include "stone/Diag/DiagnosticEmitter.h"
#include "stone/Diag/DiagnosticConsumer.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticConsumer;

DiagnosticEmitter::DiagnosticEmitter(DiagnosticFormatter &formatter)
    : formatter(formatter) {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}