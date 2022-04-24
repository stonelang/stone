#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener()
    : TextDiagnosticListener(std::make_unique<TextDiagnosticEmitter>()) {}

TextDiagnosticListener::TextDiagnosticListener(
    std::unique_ptr<TextDiagnosticEmitter> custom)
    : emitter(std::move(custom)) {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::OnDiagnostic(const DiagnosticEvent &diagEvent) {

  // DiagnosticListener::OnDiagnostic(diagEvent);

  assert(emitter);
  emitter->EmitDiagnostic(diagEvent);
}

void TextDiagnosticListener::Finish() {}

void TextDiagnosticListener::Flush() {}
