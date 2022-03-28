#ifndef STONE_CORE_DIAGNOSTICEMITTER_H
#define STONE_CORE_DIAGNOSTICEMITTER_H

#include "stone/Core/DiagnosticFormatter.h"

#include <assert.h>

namespace stone {
class DiagnosticFormatter;
class DiagnosticEvent;

class DiagnosticEmitter {

protected:
  DiagnosticFormatter formatter;

public:
  DiagnosticEmitter();
  virtual ~DiagnosticEmitter();

  void SetFormatter(DiagnosticFormatter &&diagFormatter) {
    formatter = diagFormatter;
  }
  DiagnosticFormatter &GetFormatter() { return formatter; }

public:
  virtual void EmitLevel();
  virtual void EmitDiagnostic(const DiagnosticEvent &diagnostic);
  virtual void EmitLoc();
};
} // namespace stone
#endif
