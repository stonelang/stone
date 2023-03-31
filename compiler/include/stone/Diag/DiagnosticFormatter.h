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

  // TODO: Move to emitter
  ColorfulStream colorfulStream;

public:
  DiagnosticFormatter();
  virtual ~DiagnosticFormatter();

  virtual void
  Format(llvm::raw_ostream &out, DiagnosticDetail &detail,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

  virtual void
  Format(llvm::raw_ostream &out, llvm::StringRef text,
         llvm::ArrayRef<diag::Argument> args,
         DiagnosticFormatOptions fmtOpts = DiagnosticFormatOptions()) {}

public:
  ColorfulStream &GetColorfulStream() { return colorfulStream; }
};
} // namespace stone
#endif
