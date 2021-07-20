#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Core/TextDiagnosticListener.h"

using stone::TextDiagnosticEmitter;
using stone::TextDiagnosticListener;

TextDiagnosticEmitter::TextDiagnosticEmitter(TextDiagnosticListener *listener)
    : DiagnosticEmitter(listener) {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitMessage() {

    
}

void TextDiagnosticEmitter::EmitLoc() {}