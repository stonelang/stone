#include "stone/Utils/DiagnosticEmitter.h"
#include "stone/Utils/DiagnosticListener.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticListener;

DiagnosticEmitter::DiagnosticEmitter(DiagnosticListener *listener)
    : listener(listener) {}

DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitMessage() {}
void DiagnosticEmitter::EmitLoc() {}