#ifndef STONE_DIAG_TEXTDIAGNOSTICEMITTER_H
#define STONE_DIAG_TEXTDIAGNOSTICEMITTER_H

#include "stone/Diag/DiagnosticEmitter.h"
#include "stone/Diag/TextDiagnosticFormatter.h"

namespace stone {
class DiagnosticEvent;

class TextDiagnosticEmitter : public DiagnosticEmitter {
  std::unique_ptr<TextDiagnosticFormatter> formatter;

public:
  TextDiagnosticEmitter();
  TextDiagnosticEmitter(std::unique_ptr<TextDiagnosticFormatter> custom);
  virtual ~TextDiagnosticEmitter();

public:
  virtual void EmitLevel() override;
  virtual void EmitDiagnostic(const DiagnosticEvent &diagnostic) override;
  virtual void EmitLoc() override;
};
} // namespace stone
#endif