#ifndef STONE_DIAG_TEXTDIAGNOSTICFORMATTER_H
#define STONE_DIAG_TEXTDIAGNOSTICFORMATTER_H

#include <assert.h>

#include "stone/Diag/DiagnosticFormatter.h"

namespace stone {

class TextDiagnosticFormatter : public DiagnosticFormatter {
public:
  TextDiagnosticFormatter();
  ~TextDiagnosticFormatter();

public:
  void
  Format(llvm::raw_ostream &out, DiagnosticDetail &detail,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

  void
  Format(llvm::raw_ostream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) override;

public:
};
} // namespace stone
#endif
