#include "stone/Core/TextDiagnosticListener.h"

#include "stone/Core/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener() {
  emitter.reset(new TextDiagnosticEmitter(this));
}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::Listen(diag::Level level,
                                    const EmissionDiagnostic &diagnostic) {
  DiagnosticListener::Listen(level, diagnostic);
  emitter->EmitMessage();
}

void TextDiagnosticListener::Finish() {}

void TextDiagnosticListener::Flush() {}
