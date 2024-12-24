#include "stone/Diag/TextDiagnosticPrinter.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticText.h"
using namespace stone;

TextDiagnosticPrinter::TextDiagnosticPrinter(llvm::raw_ostream &stream)
    : OS(stream) {}

TextDiagnosticPrinter::~TextDiagnosticPrinter() { OS.Flush(); }

void TextDiagnosticPrinter::PrintDiagnostic() {}

void TextDiagnosticPrinter::EmitDiagnostic() {}

void TextDiagnosticPrinter::HandleDiagnostic(DiagnosticEngine &DE,
                                             const DiagnosticInfo &DI) {

  /// Make sure that it has a message.
  assert(DI && "unable print a dianostic without a message!");

  DiagnosticEngine::FormatDiagnosticText(OS.GetOutputStream(), DE.GetSrcMgr(),
                                         DI);

  // DE.GetFormatter().FormatDiagnostic(OS.GetOutputStream(), DE.GetSrcMgr(),
  // DC);

  // 1. Format message
  //  Default implementation (Warnings/errors count).
  // DiagnosticClient::HandleDiagnostic(SM, DI);
  // // Render the diagnostic message into a temporary buffer eagerly. We'll use
  // // this later as we print out the diagnostic to the terminal.
  // DI.FormatDiagnostic(OS.GetOutputStream(), SM);

  // 2. Print the formatted message
  //  // Keeps track of the starting position of the location
  //  // information (e.g., "foo.c:10:4:") that precedes the error
  //  // message. We use this information to determine how long the
  //  // file+line+column number prefix is.
  //  uint64_t StartOfLocationInfo = OS.tell();

  // if (!Prefix.empty()) {
  //   OS << Prefix << ": ";
  // }

  // Use a dedicated, simpler path for diagnostics without a valid location.
  // This is important as if the location is missing, we may be emitting
  // diagnostics in a context that lacks language options, a source manager, or
  // other infrastructure necessary when emitting more rich diagnostics.
  // if (!Info.getLocation().isValid()) {
  //   TextDiagnostic::printDiagnosticLevel(OS, Level, DiagOpts->ShowColors);
  //   TextDiagnostic::printDiagnosticMessage(
  //       OS, /*IsSupplemental=*/Level == DiagnosticsEngine::Note,
  //       DiagMessageStream.str(), OS.tell() - StartOfLocationInfo,
  //       DiagOpts->MessageLength, DiagOpts->ShowColors);
  //   OS.flush();
  //   return;
  // }

  // // Assert that the rest of our infrastructure is setup properly.
  // assert(DiagOpts && "Unexpected diagnostic without options set");
  // assert(Info.hasSourceManager() &&
  //        "Unexpected diagnostic with no source manager");
  // assert(TextDiag && "Unexpected diagnostic outside source file processing");

  // GetEmitter().EmitDiagnostic(FullSrcLoc(Info.GetLoc(), Info.GetSrcMgr()),
  //                             Level, DiagMessageStream.str(),
  //                             Info.GetRanges(), Info.GetFixIts());

  // GetEmitter().GetDiagnosticOutputStream().Flush();

  // OS.Flush();

  DE.DiagnosticCompletionCallback(DI);
}

TextDiagnosticPrinterImpl::TextDiagnosticPrinterImpl() {}

TextDiagnosticPrinterImpl::~TextDiagnosticPrinterImpl() {}

void TextDiagnosticPrinterImpl::HandleDiagnostic(DiagnosticEngine &DE,
                                                 const DiagnosticInfo &D) {

  // Default implementation (Warnings/errors count).
  // TextDiagnosticPrinter::HandleDiagnostic(SM, DI);

  // PrintDiagnostic(); if there are certain conditions

  EmitDiagnostic();
}

bool TextDiagnosticPrinterImpl::FinishProcessing() { return true; }
