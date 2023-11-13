#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Diag/DiagnosticEngine.h"

using stone::Diagnostic;
using stone::TextDiagnosticConsumer;

TextDiagnosticConsumer::TextDiagnosticConsumer(TextDiagnosticEmitter &emitter)
    : DiagnosticConsumer(emitter) {}

TextDiagnosticConsumer::~TextDiagnosticConsumer() {}

void TextDiagnosticConsumer::Consume(const DiagnosticMessage &diagEvent) {

  // DiagnosticConsumer::Listen(diagEvent);

  // switch(diagEvent.GetDiagLevel()){

  // }
  GetEmitter().EmitDiagnostic(diagEvent);
}

bool TextDiagnosticConsumer::Finish() { return false; }
