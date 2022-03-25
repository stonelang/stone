#include "stone/Core/TextDiagnosticListener.h"

#include "stone/Core/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener() {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::OnDiagnostic(
    const EmissionDiagnostic &diagnostic) {
  // DiagnosticListener::Listen(level, diagnostic);
  emitter.EmitDiagnostic(diagnostic);
}

void TextDiagnosticListener::Finish() {}

void TextDiagnosticListener::Flush() {}
