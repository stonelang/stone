#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Basic/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener() {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::Listen(diag::Level level,
                                    const EmissionDiagnostic &diagnostic) {

  DiagnosticListener::Listen(level, diagnostic);

  emitter.reset(new TextDiagnosticEmitter(this));

  emitter->EmitMessage();
}
