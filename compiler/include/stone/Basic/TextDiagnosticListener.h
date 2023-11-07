#ifndef STONE_DIAG_TEXTDIAGNOSTICLISTENER_H
#define STONE_DIAG_TEXTDIAGNOSTICLISTENER_H

#include <memory>

#include "stone/Basic/DiagnosticListener.h"
#include "stone/Basic/TextDiagnosticEmitter.h"

namespace stone {
class DiagnosticMessage;

class TextDiagnosticListener : public DiagnosticListener {

public:
  TextDiagnosticListener() = delete;
  TextDiagnosticListener(TextDiagnosticEmitter &emitter);

  ~TextDiagnosticListener();

public:
  void OnDiagnostic(const DiagnosticMessage &diagnostic) override;
  bool Finish() override;

  // void ForceColors() {
  //   ForceColors = true;
  //   llvm::sys::Process::UseANSIEscapeCodes(true);
  // }
};
} // namespace stone

#endif
