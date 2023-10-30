#ifndef STONE_DIAG_DIAGNOSTICFORMATTER_H
#define STONE_DIAG_DIAGNOSTICFORMATTER_H

#include "stone/Basic/Color.h"
#include "stone/Diag/Diagnostic.h"
#include "stone/Diag/DiagnosticArgument.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

#include <assert.h>

namespace stone {
class DiagnosticDetail;

class DiagnosticFormatter {

public:
  DiagnosticFormatter();
  virtual ~DiagnosticFormatter();

  virtual void
  Format(ColorStream &out, DiagnosticDetail &detail,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

  virtual void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

  virtual void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                              llvm::StringRef modifierArguments,
                              ArrayRef<diag::Argument> args, unsigned argIndex,
                              DiagnosticFormatOptions fmtOpts) {}
};
} // namespace stone
#endif
