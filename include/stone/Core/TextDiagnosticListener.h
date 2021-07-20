#ifndef STONE_CORE_TEXTDIAGNOSTICLISTENER_H
#define STONE_CORE_TEXTDIAGNOSTICLISTENER_H

#include "stone/Core/DiagnosticListener.h"
#include "stone/Core/TextDiagnosticEmitter.h"

#include <memory>

namespace stone {
class Diagnostic;

class TextDiagnosticListener : public DiagnosticListener {

  std::unique_ptr<TextDiagnosticEmitter> emitter;

public:
  TextDiagnosticListener();
  ~TextDiagnosticListener();

public:
  void Listen(diag::Level level, const EmissionDiagnostic &diagnostic) override;

public:
  void SetEmitter(std::unique_ptr<DiagnosticEmitter> emitter);
};
} // namespace stone

#endif
