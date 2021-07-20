#ifndef STONE_UTILS_TEXTDIAGNOSTICLISTENER_H
#define STONE_UTILS_TEXTDIAGNOSTICLISTENER_H

#include "stone/Utils/DiagnosticListener.h"
#include "stone/Utils/TextDiagnosticEmitter.h"

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
