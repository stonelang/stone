#ifndef STONE_DIAG_TEXTDIAGNOSTICEMITTER_H
#define STONE_DIAG_TEXTDIAGNOSTICEMITTER_H

#include "stone/Diag/DiagnosticEmitter.h"
#include "stone/Diag/TextDiagnosticFormatter.h"

namespace stone {
class DiagnosticEvent;

class TextDiagnosticEmitter : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter() = delete;
  TextDiagnosticEmitter(TextDiagnosticFormatter &formatter);
  virtual ~TextDiagnosticEmitter();

public:
  virtual void EmitLevel() override;
  virtual void EmitDiagnostic(const DiagnosticEvent &diagnostic) override;
  virtual void EmitLoc() override;
};
} // namespace stone
#endif