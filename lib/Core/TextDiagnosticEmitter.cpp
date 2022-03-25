#include "stone/Core/TextDiagnosticEmitter.h"

#include "stone/Core/TextDiagnosticListener.h"

using stone::TextDiagnosticEmitter;
using stone::TextDiagnosticListener;

TextDiagnosticEmitter::TextDiagnosticEmitter() {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitDiagnostic(
    const EmissionDiagnostic &diagnostic) {
  printf("%s", "TextDiagnosticEmitter::EmitMessage\n");
}

void TextDiagnosticEmitter::EmitLoc() {}