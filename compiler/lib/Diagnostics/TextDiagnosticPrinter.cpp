#include "stone/Diagnostics/TextDiagnosticPrinter.h"

using namespace stone;

diags::TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream &OS,
                                                    const LangOptions &LO,
                                                    DiagnosticOptions &DiagOpts,
                                                    bool OwnsOutputStream)
    : OS(OS), LO(LO), DiagOpts(DiagOpts), OwnsOutputStream(OwnsOutputStream),
      emitter(new TextDiagnosticEmitter(OS, LO, DiagOpts)) {}

diags::TextDiagnosticPrinter::~TextDiagnosticPrinter() {}

void diags::TextDiagnosticPrinter::Reset() { emitter.reset(); }

void diags::TextDiagnosticPrinter::HandleDiagnostic(
    DiagnosticLevel Level, const DiagnosticInfo &Info) {

  // Default implementation (Warnings/errors count).
  DiagnosticClient::HandleDiagnostic(Level, Info);

  // Render the diagnostic message into a temporary buffer eagerly. We'll use
  // this later as we print out the diagnostic to the terminal.
  llvm::SmallString<100> OutStr;
  Info.FormatDiagnostic(OutStr);

}
