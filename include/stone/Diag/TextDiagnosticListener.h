#ifndef STONE_DIAG_TEXTDIAGNOSTICLISTENER_H
#define STONE_DIAG_TEXTDIAGNOSTICLISTENER_H

#include <memory>

#include "stone/Diag/DiagnosticListener.h"
#include "stone/Diag/TextDiagnosticEmitter.h"

namespace stone {
class DiagnosticEvent;

class TextDiagnosticListener : public DiagnosticListener {
  std::unique_ptr<TextDiagnosticEmitter> emitter;

public:
  TextDiagnosticListener();
  TextDiagnosticListener(std::unique_ptr<TextDiagnosticEmitter> custom);

  ~TextDiagnosticListener();

public:
  void OnDiagnostic(const DiagnosticEvent &diagnostic) override;
  void Finish() override;
  void Flush() override;

  // void ForceColors() {
  //   ForceColors = true;
  //   llvm::sys::Process::UseANSIEscapeCodes(true);
  // }
};
} // namespace stone

#endif
