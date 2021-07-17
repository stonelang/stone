#include "stone/Basic/DiagnosticEngine.h"
#include "stone/Basic/TextDiagnosticListener.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;
using stone::TextBufferingDiagnosticListener;


TextBufferingDiagnosticListener::TextBufferingDiagnosticListener() {}

void TextBufferingDiagnosticListener::Listen(diag::Level level,
                                             const Diagnostic &diagnostic) {}

TextDiagnosticListener::TextDiagnosticListener() {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::Listen(diag::Level level,
                                    const Diagnostic &diagnostic) {
  emitter->EmitMessage();
}
