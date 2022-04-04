#ifndef STONE_BASIC_TEXTDIAGNOSTICLISTENER_H
#define STONE_BASIC_TEXTDIAGNOSTICLISTENER_H

#include <memory>

#include "stone/Basic/DiagnosticListener.h"
#include "stone/Basic/TextDiagnosticEmitter.h"

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
