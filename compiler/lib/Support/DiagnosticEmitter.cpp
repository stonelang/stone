#include "stone/Support/DiagnosticEmitter.h"
#include "stone/Support/DiagnosticConsumer.h"

using stone::DiagnosticConsumer;
using stone::DiagnosticEmitter;

DiagnosticEmitter::DiagnosticEmitter(DiagnosticFormatter &formatter)
    : formatter(formatter) {}
DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitDiagnostic(const DiagnosticMessage &diagnostic) {}
void DiagnosticEmitter::EmitLoc() {}