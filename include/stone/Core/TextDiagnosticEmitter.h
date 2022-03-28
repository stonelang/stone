#ifndef STONE_CORE_TEXTDIAGNOSTICEMITTER_H
#define STONE_CORE_TEXTDIAGNOSTICEMITTER_H

#include "stone/Core/DiagnosticEmitter.h"

namespace stone {

class TextDiagnosticEmitter : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter();
  ~TextDiagnosticEmitter();

public:
  virtual void EmitLevel() override;
  virtual void EmitDiagnostic(const DiagnosticEvent &diagnostic) override;
  virtual void EmitLoc() override;
};
} // namespace stone
#endif
