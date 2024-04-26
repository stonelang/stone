#ifndef STONE_DIAG_DIAGNOSTICEMITTER_H
#define STONE_DIAG_DIAGNOSTICEMITTER_H

#include "stone/Basic/ColorStream.h"
#include "stone/Support/DiagnosticArgument.h"
#include "stone/Support/DiagnosticOptions.h"

#include <assert.h>

namespace stone {

class Diagnostic;
class DiagnosticMessage;
class DiagnosticFormatter;

class DiagnosticEmitter {
public:
  DiagnosticEmitter();
  virtual ~DiagnosticEmitter();

public:
  virtual void EmitLevel();
  virtual void EmitDiagnostic(const DiagnosticMessage &msg);
  virtual void EmitLoc();

public:
  virtual void
  Format(ColorStream &out, const Diagnostic &diagnostic,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions());

  virtual void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<DiagnosticArgument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions());

  virtual void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                              llvm::StringRef modifierArguments,
                              ArrayRef<DiagnosticArgument> args,
                              unsigned argIndex,
                              DiagnosticFormatOptions fmtOpts);
};

} // namespace stone
#endif