#include "stone/Basic/TextDiagnosticEmitter.h"
#include "stone/Basic/TextDiagnosticListener.h"

using stone::TextDiagnosticEmitter;
using stone::TextDiagnosticListener;

TextDiagnosticEmitter::TextDiagnosticEmitter(TextDiagnosticListener *listener)
    : DiagnosticEmitter(listener) {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitMessage() { printf("%s", "emit message\n"); }

void TextDiagnosticEmitter::EmitLoc() {}