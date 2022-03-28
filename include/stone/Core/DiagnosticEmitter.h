#ifndef STONE_CORE_DIAGNOSTICEMITTER_H
#define STONE_CORE_DIAGNOSTICEMITTER_H

#include "stone/Core/Color.h"
#include "stone/Core/DiagnosticFormatter.h"

#include <assert.h>

namespace stone {
class DiagnosticFormatter;
class DiagnosticEvent;

class DiagnosticEmitter {

protected:
  ColorfulStream colorfulStream;
  DiagnosticFormatter *formatter = nullptr;

public:
  DiagnosticEmitter();
  virtual ~DiagnosticEmitter();

  void SetFormatter(DiagnosticFormatter *diagFormatter) {
    assert(diagFormatter);
    formatter = diagFormatter;
  }
  DiagnosticFormatter *GetFormatter() { return formatter; }
  ColorfulStream &GetColorfulOS() { return colorfulStream; }

public:
  virtual void EmitLevel();
  virtual void EmitDiagnostic(const DiagnosticEvent &diagnostic);
  virtual void EmitLoc();
};
} // namespace stone
#endif
