#ifndef STONE_DIAG_DIAGNOSTIC_PRINTER_H
#define STONE_DIAG_DIAGNOSTIC_PRINTER_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticClient.h"
// #include "stone/Diag/DiagnosticEmitter.h"

#include <memory>

namespace stone {

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

  void PrintDiagnostic();
  void EmitDiagnostic();
  void EmitDiagnosticMessage();
  void EmitDiagnosticCaret();

  void HandleDiagnostic(DiagnosticEngine &DE,
                        const DiagnosticImpl &DC) override;

  /// \returns true if an error occurred while finishing-up.
  bool FinishProcessing() override { return true; }

  // /// Flush any in-flight diagnostics.
  // virtual void FlushInflightDiagnostic() {}
};

class TextDiagnosticPrinterImpl final : public TextDiagnosticPrinter {

public:
  TextDiagnosticPrinterImpl();
  ~TextDiagnosticPrinterImpl();

public:
  bool FinishProcessing() override;
  void HandleDiagnostic(DiagnosticEngine &DE,
                        const DiagnosticImpl &DC) override;
};

} // namespace stone

#endif