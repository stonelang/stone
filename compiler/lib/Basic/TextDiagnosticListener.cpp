#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticListener;

TextDiagnosticListener::TextDiagnosticListener(TextDiagnosticEmitter &emitter)
    : DiagnosticListener(emitter) {}

TextDiagnosticListener::~TextDiagnosticListener() {}

void TextDiagnosticListener::OnDiagnostic(const DiagnosticMessage &diagMsg) {

  // DiagnosticListener::OnDiagnostic(diagMsg);

  // switch(diagMsg.GetDiagLevel()){

  // }
  GetEmitter().EmitDiagnostic(diagMsg);
}

bool TextDiagnosticListener::Finish() { return false; }
