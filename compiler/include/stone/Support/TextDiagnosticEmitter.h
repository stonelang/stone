#ifndef STONE_DIAG_TEXTDIAGNOSTICEMITTER_H
#define STONE_DIAG_TEXTDIAGNOSTICEMITTER_H

#include "stone/Support/DiagnosticEmitter.h"
#include "stone/Support/TextDiagnosticFormatter.h"

namespace stone {
class DiagnosticMessage;

class TextDiagnosticEmitter : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter() = delete;
  TextDiagnosticEmitter(TextDiagnosticFormatter &formatter);
  virtual ~TextDiagnosticEmitter();

public:
  virtual void EmitLevel() override;
  virtual void EmitDiagnostic(const DiagnosticMessage &diagnostic) override;
  virtual void EmitLoc() override;
};
} // namespace stone
#endif