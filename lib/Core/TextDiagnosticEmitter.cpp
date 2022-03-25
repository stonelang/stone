#include "stone/Core/TextDiagnosticEmitter.h"
#include "stone/Core/Diagnostic.h"

using stone::TextDiagnosticEmitter;

TextDiagnosticEmitter::TextDiagnosticEmitter() {}

TextDiagnosticEmitter::~TextDiagnosticEmitter() {}

void TextDiagnosticEmitter::EmitLevel() {}

void TextDiagnosticEmitter::EmitDiagnostic(
    const EmissionDiagnostic &diagnostic) {

  auto ed = const_cast<EmissionDiagnostic &>(diagnostic);
  printf("%s", ed.GetFormatMessage().data());
}

void TextDiagnosticEmitter::EmitLoc() {}