#ifndef STONE_DIAG_DIAGNOSTICFORMATTER_H
#define STONE_DIAG_DIAGNOSTICFORMATTER_H

#include "stone/Diag/DiagnosticClient.h"

namespace stone {

struct DiagnosticImpl;
class DiagnosticTextFormatter {

public:
  DiagnosticTextFormatter();
  virtual ~DiagnosticTextFormatter();

public:
  virtual bool FormatDiagnosticText(
      llvm::raw_ostream &OS, SrcMgr &SM, const DiagnosticImpl &DC,
      DiagnosticFormatOptions FormatOpts = DiagnosticFormatOptions()) const;
};

} // namespace stone

#endif