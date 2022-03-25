#ifndef STONE_CORE_TEXTDIAGNOSTICLISTENER_H
#define STONE_CORE_TEXTDIAGNOSTICLISTENER_H

#include <memory>

#include "stone/Core/DiagnosticListener.h"
#include "stone/Core/TextDiagnosticEmitter.h"

namespace stone {
class EmissionDiagnostic;

class TextDiagnosticListener : public DiagnosticListener {
  TextDiagnosticEmitter emitter;

public:
  TextDiagnosticListener();
  ~TextDiagnosticListener();

public:
  void OnDiagnostic(const EmissionDiagnostic &diagnostic) override;
  void Finish() override;
  void Flush() override;

public:
  void SetEmitter(std::unique_ptr<DiagnosticEmitter> emitter);
};
} // namespace stone

#endif
