#ifndef STONE_DIAG_TEXTDIAGNOSTIICCONSUMER_H
#define STONE_DIAG_TEXTDIAGNOSTIICCONSUMER_H

#include <memory>

#include "stone/Support/DiagnosticConsumer.h"
#include "stone/Support/TextDiagnosticEmitter.h"

namespace stone {
class DiagnosticMessage;

class TextDiagnosticConsumer : public DiagnosticConsumer {
public:
  TextDiagnosticConsumer(
      TextDiagnosticEmitter emitter = TextDiagnosticEmitter());
  ~TextDiagnosticConsumer();

public:
  void ConsumeDiagnostic(const DiagnosticMessage &msg,
               DiagnosticFormatter *formatter = nullptr) override;
  bool FinishProcessing() override;

  // void ForceColors() {
  //   ForceColors = true;
  //   llvm::sys::Process::UseANSIEscapeCodes(true);
  // }
};
} // namespace stone

#endif
