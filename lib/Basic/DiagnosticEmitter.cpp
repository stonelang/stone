#include "stone/Basic/DiagnosticEmitter.h"
#include "stone/Basic/DiagnosticListener.h"

using stone::DiagnosticEmitter;
using stone::DiagnosticListener;

DiagnosticEmitter::DiagnosticEmitter(DiagnosticListener *listener)
    : listener(listener) {}

DiagnosticEmitter::~DiagnosticEmitter() {}

void DiagnosticEmitter::EmitLevel() {}
void DiagnosticEmitter::EmitMessage() {}
void DiagnosticEmitter::EmitLoc() {}