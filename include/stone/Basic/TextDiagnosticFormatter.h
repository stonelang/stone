#ifndef STONE_BASIC_TEXTDIAGNOSTICFORMATTER_H
#define STONE_BASIC_TEXTDIAGNOSTICFORMATTER_H

#include <assert.h>

#include "stone/Basic/DiagnosticFormatter.h"

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
