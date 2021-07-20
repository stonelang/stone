#ifndef STONE_BASIC_DIAGNOSTICEMITTER_H
#define STONE_BASIC_DIAGNOSTICEMITTER_H

namespace stone {
class DiagnosticEmitter {
public:
  virtual ~DiagnosticEmitter();

public:
  virtual void EmitLevel();
  virtual void EmitMessage();
  virtual void EmitLoc();
};
} // namespace stone
#endif
