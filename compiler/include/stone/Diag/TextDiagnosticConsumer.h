#ifndef STONE_DIAG_TEXTDIAGNOSTIICCONSUMER_H
#define STONE_DIAG_TEXTDIAGNOSTIICCONSUMER_H

#include <memory>

#include "stone/Diag/DiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticEmitter.h"

namespace stone {
class DiagnosticMessage;

class TextDiagnosticConsumer : public DiagnosticConsumer {

public:
  TextDiagnosticConsumer() = delete;
  TextDiagnosticConsumer(TextDiagnosticEmitter &emitter);

  ~TextDiagnosticConsumer();

public:
  void Consume(const DiagnosticMessage &msg) override;
  void Consume(const TextDiagnosticEmitter &emiiter,
               const DiagnosticMessage &msg) override;
  bool Finish() override;

  // void ForceColors() {
  //   ForceColors = true;
  //   llvm::sys::Process::UseANSIEscapeCodes(true);
  // }
};
} // namespace stone

#endif
