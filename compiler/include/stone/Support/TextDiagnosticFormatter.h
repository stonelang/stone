#ifndef STONE_DIAG_TEXTDIAGNOSTICFORMATTER_H
#define STONE_DIAG_TEXTDIAGNOSTICFORMATTER_H

#include <assert.h>

#include "stone/Support/DiagnosticFormatter.h"

namespace stone {

class TextDiagnosticFormatter : public DiagnosticFormatter {
public:
  TextDiagnosticFormatter();
  ~TextDiagnosticFormatter();

public:
  void
  Format(ColorStream &out, const Diagnostic &diagnostic,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void
  Format(ColorStream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void FormatArgument(ColorStream &out, llvm::StringRef modifier,
                      llvm::StringRef modifierArguments,
                      ArrayRef<diag::Argument> args, unsigned argIndex,
                      DiagnosticFormatOptions fmtOpts) override;

public:
};
} // namespace stone
#endif
