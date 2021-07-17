#ifndef STONE_BASIC_TEXTDIAGNOSTICLISTENER_H
#define STONE_BASIC_TEXTDIAGNOSTICLISTENER_H

#include "stone/Basic/DiagnosticListener.h"
#include "stone/Basic/TextDiagnosticEmitter.h"

#include <memory>

namespace stone {
class Diagnostic;

class TextBufferingDiagnosticListener final : public DiagnosticListener {
public:
  TextBufferingDiagnosticListener();

public:
  void Listen(diag::Level, const Diagnostic &diagnostic) override;
};

class TextDiagnosticListener final : public DiagnosticListener {

  std::unique_ptr<TextDiagnosticEmitter> emitter;

public:
  TextDiagnosticListener();
  ~TextDiagnosticListener();

public:
  void Listen(diag::Level, const Diagnostic &diagnostic) override;

public:
  void SetEmitter(std::unique_ptr<DiagnosticEmitter> emitter);
};
} // namespace stone

#endif
