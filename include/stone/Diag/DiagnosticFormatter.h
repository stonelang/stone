#ifndef STONE_DIAG_DIAGNOSTICFORMATTER_H
#define STONE_DIAG_DIAGNOSTICFORMATTER_H

#include "stone/Diag/DiagnosticClient.h"

namespace stone {

struct DiagnosticContext;
class DiagnosticFormatter {

public:
  DiagnosticFormatter();
  virtual ~DiagnosticFormatter();

public:
  virtual bool FormatDiagnostic(
      llvm::raw_ostream &OS, SrcMgr &SM, const DiagnosticContext &DC,
      DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions()) const;
};

} // namespace stone

#endif