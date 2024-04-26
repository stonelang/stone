#include "stone/Support/TextDiagnosticConsumer.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Support/DiagnosticEngine.h"

using namespace stone;

TextDiagnosticConsumer::TextDiagnosticConsumer()
    : DiagnosticConsumer(emitter) {}

TextDiagnosticConsumer::~TextDiagnosticConsumer() {}

void TextDiagnosticConsumer::Consume(const DiagnosticMessage &msg,
                                     DiagnosticFormatter *formatter) {
  if (formatter) {
    // use this one else use DiagnosticEngine::Format(msg)
  }
  // DiagnosticConsumer::Listen(diagEvent);
  // switch(diagEvent.GetDiagLevel()){
  // }
  // GetEmitter().EmitDiagnostic(msg);
}

bool TextDiagnosticConsumer::Finish() { return false; }
