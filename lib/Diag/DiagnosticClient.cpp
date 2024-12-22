#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"
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
                                               const DiagnosticImpl &DI) {
  if (!UseInDiagnosticCounts()) {
    return;
  }

  // if (DiagLevel == DiagnosticsEngine::Warning)
  //   ++NumWarnings;
  // else if (DiagLevel >= DiagnosticsEngine::Error)
  //   ++NumErrors;
}

// diags::DiagID diags::Diagnostic::GetDiagID() const {
//   return DE->GetCurDiagID();
// }

diags::DiagnosticImpl::DiagnosticImpl(DiagnosticKind Kind,
                                      const Diagnostic *Diag)
    : DiagnosticImpl(Kind, Diag, llvm::errs()) {}

diags::DiagnosticImpl::DiagnosticImpl(DiagnosticKind Kind,
                                      const Diagnostic *Diag,
                                      llvm::raw_ostream &OS)
    : Kind(Kind), Diag(Diag), OS(OS) {}

void diags::DiagnosticImpl::FormatDiagnostic(
    DiagnosticFormatOptions FormatOpts) const {

  FormatDiagnostic(OS);
}

void diags::DiagnosticImpl::FormatDiagnostic(
    llvm::raw_ostream &Out, DiagnosticFormatOptions FormatOpts) const {

  // DiagnosticEngine::FormatDiagnosticText()
}