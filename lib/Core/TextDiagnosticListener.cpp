#include "stone/Core/TextDiagnosticListener.h"

#include "stone/Core/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener() {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::OnDiagnostic(const DiagnosticEvent &diagnostic) {
  // DiagnosticListener::OnDiagnostic(level, diagnostic);

  assert(emitter);
  emitter->EmitDiagnostic(diagnostic);
}

void TextDiagnosticListener::Finish() {}

void TextDiagnosticListener::Flush() {}
