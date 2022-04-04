#ifndef STONE_BASIC_DIAGNOSTICEMITTER_H
#define STONE_BASIC_DIAGNOSTICEMITTER_H

#include "stone/Basic/Color.h"
#include "stone/Basic/DiagnosticFormatter.h"

#include <assert.h>

namespace stone {
class DiagnosticFormatter;
class DiagnosticEvent;

class DiagnosticEmitter {

public:
  DiagnosticEmitter();
  virtual ~DiagnosticEmitter();

public:
  virtual void EmitLevel();
  virtual void EmitDiagnostic(const DiagnosticEvent &diagnostic);
  virtual void EmitLoc();
};

} // namespace stone
#endif