#ifndef STONE_BASIC_TEXTDIAGNOSTICLISTENER_H
#define STONE_BASIC_TEXTDIAGNOSTICLISTENER_H

#include "stone/Basic/DiagnosticListener.h"
#include "stone/Basic/TextDiagnosticEmitter.h"

#include <memory>

namespace stone {
class EmissionDiagnostic;
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
