#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener(TextDiagnosticEmitter &emitter)
    : DiagnosticListener(emitter) {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::OnDiagnostic(const DiagnosticMessage &diagEvent) {

  // DiagnosticListener::OnDiagnostic(diagEvent);

  // switch(diagEvent.GetDiagLevel()){

  // }
  GetEmitter().EmitDiagnostic(diagEvent);
}

bool TextDiagnosticListener::Finish() { return false; }
