#ifndef STONE_DIAG_TEXTDIAGNOSTIICCONSUMER_H
#define STONE_DIAG_TEXTDIAGNOSTIICCONSUMER_H

#include <memory>

#include "stone/Support/DiagnosticConsumer.h"
#include "stone/Support/TextDiagnosticEmitter.h"

namespace stone {
class DiagnosticMessage;

class TextDiagnosticConsumer : public DiagnosticConsumer {

  TextDiagnosticEmitter emitter;

public:
  TextDiagnosticConsumer();
  ~TextDiagnosticConsumer();

public:
  void Consume(const DiagnosticMessage &msg,
               DiagnosticFormatter *formatter = nullptr) override;
  bool Finish() override;

  // void ForceColors() {
  //   ForceColors = true;
  //   llvm::sys::Process::UseANSIEscapeCodes(true);
  // }
};
} // namespace stone

#endif
