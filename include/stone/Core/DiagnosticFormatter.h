#ifndef STONE_CORE_DIAGNOSTICFORMATTER_H
#define STONE_CORE_DIAGNOSTICFORMATTER_H

#include <assert.h>

namespace stone {
class DiagnosticDetail;
class DiagnosticFormatOptions;

class DiagnosticFormatter {

public:
  DiagnosticFormatter();
  virtual ~DiagnosticFormatter();

  // virtual void FormatText(
  //       llvm::raw_ostream &result, llvm::StringRef textToFormat,
  //       llvm::ArrayRef<DiagnosticArgument> formatArgs,
  //       DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions());

  virtual void FormatText(DiagnosticDetail &detail,
                          DiagnosticFormatOptions &fmtOptions) {}

public:
};
} // namespace stone
#endif
