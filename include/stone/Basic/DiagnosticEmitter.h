#ifndef STONE_BASIC_DIAGNOSTICEMITTER_H
#define STONE_BASIC_DIAGNOSTICEMITTER_H

namespace stone {
class DiagnosticEmitter {
public:
  virtual void EmitDiagnosticLevel();
  virtual void EmitDiagnosticMessage();
  virtual void EmitDiagnosticLoc();
};
} // namespace stone
#endif
