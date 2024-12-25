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

  /// Print the diagonstic level to a raw_ostream.
  ///
  /// This is a static helper that handles colorizing the level and formatting
  /// it into an arbitrary output stream. This is used internally by the
  /// TextDiagnostic emission code, but it can also be used directly by
  /// consumers that don't have a source manager or other state that the full
  /// TextDiagnostic logic requires.
  virtual void PrintDiagnosticKind(raw_ostream &OS, DiagnosticKind Kind,
                                   bool ShowColors) {}
  void PrintDiagnostic();
  void EmitDiagnostic();
  void EmitDiagnosticMessage();
  void EmitDiagnosticCaret();

  void HandleDiagnostic(DiagnosticEngine &DE,
                        const DiagnosticInfo &DI) override;

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
                        const DiagnosticInfo &DI) override;
};

} // namespace stone

#endif