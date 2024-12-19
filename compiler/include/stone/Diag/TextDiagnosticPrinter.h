#ifndef STONE_DIAG_DIAGNOSTIC_PRINTER_H
#define STONE_DIAG_DIAGNOSTIC_PRINTER_H

#include "stone/Basic/LLVM.h"
#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/TextDiagnosticEmitter.h"
#include "stone/Support/DiagnosticOptions.h"

#include <memory>

namespace stone {
namespace diags {

class TextDiagnosticPrinter final : public DiagnosticClient {

  llvm::raw_ostream &OS;

  const LangOptions &LO;

  DiagnosticOptions &DiagOpts;

  std::unique_ptr<TextDiagnosticEmitter> emitter;

  /// A string to prefix to error messages.
  std::string Prefix;

  unsigned OwnsOutputStream : 1;

public:
  TextDiagnosticPrinter(llvm::raw_ostream &OS, const LangOptions &LO,
                        DiagnosticOptions &DiagOpts,
                        bool OwnsOutputStream = false);
  ~TextDiagnosticPrinter() override;

public:
  TextDiagnosticEmitter &GetEmitter() { return *emitter; }
  /// SetPrefix - Set the diagnostic printer prefix string, which will be
  /// printed at the start of any diagnostics. If empty, no prefix string is
  /// used.
  void SetPrefix(std::string Value) { Prefix = std::move(Value); }

  /// Reset the TextDiagnosticEmitter
  void Reset();

  void HandleDiagnostic(DiagnosticLevel Level,
                        const DiagnosticInfo &Info) override;
};

} // namespace diags

} // namespace stone

#endif