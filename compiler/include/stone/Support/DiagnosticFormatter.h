#ifndef STONE_DIAG_DIAGNOSTICFORMATTER_H
#define STONE_DIAG_DIAGNOSTICFORMATTER_H

#include "stone/Basic/ColorStream.h"
#include "stone/Support/Diagnostic.h"
#include "stone/Support/DiagnosticArgument.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

#include <assert.h>

namespace stone {
class Diagnostic;

class DiagnosticFormatter {

public:
  DiagnosticFormatter();
  virtual ~DiagnosticFormatter();

  virtual void
  Format(ColorStream &out, const Diagnostic &diagnostic,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

  virtual void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<DiagnosticArgument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

  virtual void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                              llvm::StringRef modifierArguments,
                              ArrayRef<DiagnosticArgument> args,
                              unsigned argIndex,
                              DiagnosticFormatOptions fmtOpts) {}
};
} // namespace stone
#endif
