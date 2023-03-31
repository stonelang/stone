#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener(TextDiagnosticEmitter &emitter)
    : DiagnosticListener(emitter) {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::OnDiagnostic(const DiagnosticEvent &diagEvent) {

  // DiagnosticListener::OnDiagnostic(diagEvent);
  GetEmitter().EmitDiagnostic(diagEvent);
}

void TextDiagnosticListener::Finish() {}

void TextDiagnosticListener::Flush() {}
