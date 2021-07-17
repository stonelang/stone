#ifndef STONE_BASIC_TEXTDIAGNOSTICLISTENER_H
#define STONE_BASIC_TEXTDIAGNOSTICLISTENER_H

#include "stone/Basic/DiagnosticListener.h"

namespace stone {
class Diagnostic;

class TextBufferingDiagnosticListener final : public DiagnosticListener {
public:
  TextBufferingDiagnosticListener();

public:
  void Listen(diag::Level, const Diagnostic &diagnostic) override;
};

class TextDiagnosticListener final : public DiagnosticListener {
public:
  TextDiagnosticListener();
  ~TextDiagnosticListener();

public:
  void Listen(diag::Level, const Diagnostic &diagnostic) override;
};
} // namespace stone

#endif
