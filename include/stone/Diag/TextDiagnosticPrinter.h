#ifndef STONE_DIAG_DIAGNOSTIC_PRINTER_H
#define STONE_DIAG_DIAGNOSTIC_PRINTER_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticClient.h"
// #include "stone/Diag/DiagnosticEmitter.h"

#include <memory>

namespace stone {
namespace diags {


class TextDiagnosticPrinter : public DiagnosticClient {

protected:
  DiagnosticOutputStream OS;

  DiagnosticOptions::FormattingStyle FmtStyle =
      DiagnosticOptions::FormattingStyle::LLVM;

public:
  TextDiagnosticPrinter(llvm::raw_ostream &stream = llvm::errs());
  ~TextDiagnosticPrinter();

public:
  DiagnosticOptions::FormattingStyle GetFormattingStyle() { return FmtStyle; }
  void SetFormattingStyle(DiagnosticOptions::FormattingStyle fmtStyle) {
    FmtStyle = fmtStyle;
  }
  void HandleDiagnostic(SrcMgr &SM, const DiagnosticImpl &DI) override;
};

class TextDiagnosticPrinterImpl final : public TextDiagnosticPrinter {

public:
  TextDiagnosticPrinterImpl();
  ~TextDiagnosticPrinterImpl();

public:
  void HandleDiagnostic(SrcMgr &SM, const DiagnosticImpl &DI) override;
};

} // namespace diags

} // namespace stone

#endif