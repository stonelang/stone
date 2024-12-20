#include "stone/Diag/DiagnosticClient.h"

using namespace stone;

diags::DiagnosticOutputStream::DiagnosticOutputStream()
    : DiagnosticOutputStream(llvm::errs()) {}

diags::DiagnosticOutputStream::DiagnosticOutputStream(llvm::raw_ostream &OS)
    : OS(OS), showColors(false) {
  if (OS.has_colors()) {
    UseNoteColor();
  }
}
void diags::DiagnosticOutputStream::ResetColors() { OS.resetColor(); }

void diags::DiagnosticOutputStream::UseNoteColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::CYAN);
}

void diags::DiagnosticOutputStream::UseRemarkColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::BLUE);
}

void diags::DiagnosticOutputStream::UseWarningColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::MAGENTA);
}

void diags::DiagnosticOutputStream::UseErrorColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::RED);
}

void diags::DiagnosticOutputStream::UseFatalColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::RED);
}

void diags::DiagnosticOutputStream::UseFixItColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::GREEN);
}

void diags::DiagnosticOutputStream::UseCaretColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::GREEN);
}

void diags::DiagnosticOutputStream::UseTemplateColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::CYAN);
}

void diags::DiagnosticOutputStream::UseSavedColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::SAVEDCOLOR);
}

void diags::DiagnosticOutputStream::UseCommentColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::YELLOW);
}

void diags::DiagnosticOutputStream::UseLiteralColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::GREEN);
}

void diags::DiagnosticOutputStream::UseKeywordColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::BLUE);
}

diags::DiagnosticOutputStream::~DiagnosticOutputStream() { ResetColors(); }

diags::DiagnosticClient::DiagnosticClient() {}

diags::DiagnosticClient::~DiagnosticClient() = default;

/// IncludeInDiagnosticCounts - This method (whose default implementation
///  returns true) indicates whether the diagnostics handled by this
///  DiagnosticConsumer should be included in the number of diagnostics
///  reported by DiagnosticsEngine.
bool diags::DiagnosticClient::UseInDiagnosticCounts() const { return true; }

void diags::DiagnosticClient::HandleDiagnostic(DiagnosticLevel DiagLevel,
                                               const DiagnosticInfo &Info) {
  if (!UseInDiagnosticCounts()) {
    return;
  }

  // if (DiagLevel == DiagnosticsEngine::Warning)
  //   ++NumWarnings;
  // else if (DiagLevel >= DiagnosticsEngine::Error)
  //   ++NumErrors;
}
