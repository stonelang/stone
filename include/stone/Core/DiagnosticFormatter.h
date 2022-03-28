#ifndef STONE_CORE_DIAGNOSTICFORMATTER_H
#define STONE_CORE_DIAGNOSTICFORMATTER_H

#include "stone/Core/Diagnostic.h"
#include "stone/Core/DiagnosticArgument.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

#include <assert.h>

namespace stone {
class DiagnosticDetail;

class DiagnosticFormatter {
public:
  DiagnosticFormatter();
  virtual ~DiagnosticFormatter();

  // virtual void FormatText(
  //       llvm::raw_ostream &result, llvm::StringRef textToFormat,
  //       llvm::ArrayRef<DiagnosticArgument> formatArgs,
  //       DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions());

  virtual void
  Format(llvm::raw_ostream &out, DiagnosticDetail &detail,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

  virtual void
  Format(llvm::raw_ostream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

public:
};
} // namespace stone
#endif
