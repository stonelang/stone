#ifndef STONE_CORE_DIAGNOSTICEMITTER_H
#define STONE_CORE_DIAGNOSTICEMITTER_H

#include <assert.h>

namespace stone {
class EmissionDiagnostic;
class DiagnosticEmitter {

public:
  DiagnosticEmitter();
  virtual ~DiagnosticEmitter();

public:
  virtual void EmitLevel();
  virtual void EmitDiagnostic(const EmissionDiagnostic &diagnostic);
  virtual void EmitLoc();
};
} // namespace stone
#endif
