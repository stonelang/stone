#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Basic/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextBufferingDiagnosticListener;
using stone::TextDiagnosticListener;

TextBufferingDiagnosticListener::TextBufferingDiagnosticListener() {}

void TextBufferingDiagnosticListener::Listen(diag::Level level,
                                             const Diagnostic &diagnostic) {}

TextDiagnosticListener::TextDiagnosticListener() {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::Listen(diag::Level level,
                                    const Diagnostic &diagnostic) {
  emitter->EmitMessage();
}
