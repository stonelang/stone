#ifndef STONE_DIAG_TEXTDIAGNOSTICEMITTER_H
#define STONE_DIAG_TEXTDIAGNOSTICEMITTER_H

#include "stone/Support/DiagnosticEmitter.h"

namespace stone {
class DiagnosticMessage;

class TextDiagnosticEmitter : public DiagnosticEmitter {
public:
  TextDiagnosticEmitter();
  virtual ~TextDiagnosticEmitter();

public:
  virtual void EmitLevel() override;
  virtual void EmitDiagnostic(const DiagnosticMessage &diagnostic) override;
  virtual void EmitLoc() override;

public:
  void
  Format(ColorStream &out, const Diagnostic &diagnostic,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<DiagnosticArgument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                      llvm::StringRef modifierArguments,
                      ArrayRef<DiagnosticArgument> args, unsigned argIndex,
                      DiagnosticFormatOptions fmtOpts) override;
};
} // namespace stone
#endif