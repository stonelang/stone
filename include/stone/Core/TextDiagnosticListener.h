#ifndef STONE_CORE_TEXTDIAGNOSTICLISTENER_H
#define STONE_CORE_TEXTDIAGNOSTICLISTENER_H

#include <memory>

#include "stone/Core/DiagnosticListener.h"
#include "stone/Core/TextDiagnosticEmitter.h"

namespace stone {
class DiagnosticEvent;

class TextDiagnosticListener : public DiagnosticListener {
public:
  TextDiagnosticListener();
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
