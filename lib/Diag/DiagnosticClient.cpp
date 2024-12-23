#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"
using namespace stone;

DiagnosticOutputStream::DiagnosticOutputStream()
    : DiagnosticOutputStream(llvm::errs()) {}

DiagnosticOutputStream::DiagnosticOutputStream(llvm::raw_ostream &OS)
    : OS(OS), showColors(false) {
  if (OS.has_colors()) {
    UseNoteColor();
  }
}
void DiagnosticOutputStream::ResetColors() { OS.resetColor(); }

void DiagnosticOutputStream::UseNoteColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::CYAN);
}

void DiagnosticOutputStream::UseRemarkColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::BLUE);
}

void DiagnosticOutputStream::UseWarningColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::MAGENTA);
}

void DiagnosticOutputStream::UseErrorColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::RED);
}

void DiagnosticOutputStream::UseFatalColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::RED);
}

void DiagnosticOutputStream::UseFixItColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::GREEN);
}

void DiagnosticOutputStream::UseCaretColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::GREEN);
}

void DiagnosticOutputStream::UseTemplateColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::CYAN);
}

void DiagnosticOutputStream::UseSavedColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::SAVEDCOLOR);
}

void DiagnosticOutputStream::UseCommentColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::YELLOW);
}

void DiagnosticOutputStream::UseLiteralColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::GREEN);
}

void DiagnosticOutputStream::UseKeywordColor() {
  if (showColors)
    ChangeColor(llvm::raw_ostream::Colors::BLUE);
}

DiagnosticOutputStream::~DiagnosticOutputStream() { ResetColors(); }

DiagnosticClient::DiagnosticClient() {}

DiagnosticClient::~DiagnosticClient() = default;

/// IncludeInDiagnosticCounts - This method (whose default implementation
///  returns true) indicates whether the diagnostics handled by this
///  DiagnosticConsumer should be included in the number of diagnostics
///  reported by DiagnosticsEngine.
bool DiagnosticClient::UseInDiagnosticCounts() const { return true; }

void DiagnosticClient::HandleDiagnostic(SrcMgr &SM, const DiagnosticImpl &DI) {
  // if (UseInDiagnosticCounts()) {
  //   if (DI.IsWarning()) {
  //     ++TotalWarning;
  //     return;
  //   }
  //   if (DI.IsError()) {
  //     ++TotalErrors;
  //     return;
  //   }
  // }
}

// DiagID Diagnostic::GetDiagID() const {
//   return DE->GetCurDiagID();
// }

// void DiagnosticImpl::FormatDiagnostic(
//     DiagnosticFormatOptions FormatOpts) const {

//   //FormatDiagnostic(OS);
// }

void DiagnosticImpl::FormatDiagnostic(
    llvm::raw_ostream &Out, DiagnosticFormatOptions FormatOpts) const {

  // DiagnosticEngine::FormatDiagnosticText()
}