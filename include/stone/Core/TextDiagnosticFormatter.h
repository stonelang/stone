#ifndef STONE_CORE_TEXTDIAGNOSTICFORMATTER_H
#define STONE_CORE_TEXTDIAGNOSTICFORMATTER_H

#include <assert.h>

#include "stone/Core/DiagnosticFormatter.h"

namespace stone {

class TextDiagnosticFormatter : public DiagnosticFormatter {
public:
  TextDiagnosticFormatter();
  ~TextDiagnosticFormatter();

public:
  void Format(llvm::raw_ostream &out, DiagnosticDetail &detail,
              DiagnosticFormatOptions &fmtOptions) override;

  void Format(llvm::raw_ostream &out, llvm::StringRef text,
              llvm::ArrayRef<diag::Argument> args,
              DiagnosticFormatOptions &fmtOpts) override;

public:
};
} // namespace stone
#endif
