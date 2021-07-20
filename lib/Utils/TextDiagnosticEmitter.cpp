#include "stone/Utils/TextDiagnosticEmitter.h"
#include "stone/Utils/TextDiagnosticListener.h"

using stone::TextDiagnosticEmitter;
using stone::TextDiagnosticListener;

TextDiagnosticEmitter::TextDiagnosticEmitter(TextDiagnosticListener *listener)
    : DiagnosticEmitter(listener) {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitMessage() {

    
}

void TextDiagnosticEmitter::EmitLoc() {}