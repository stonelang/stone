#include "stone/Diag/TextDiagnosticPrinter.h"

using namespace stone;

using namespace stone;

diags::TextDiagnosticPrinter::TextDiagnosticPrinter(
    DiagnosticOutputStream &OS, const LangOptions &LO,
    const DiagnosticOptions &DiagOpts)
    : emitter(new TextDiagnosticEmitter(OS, LO, DiagOpts)) {}

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

  llvm::raw_ostream &OS =
      GetEmitter().GetDiagnosticOutputStream().GetOutputStream();

  // Keeps track of the starting position of the location
  // information (e.g., "foo.c:10:4:") that precedes the error
  // message. We use this information to determine how long the
  // file+line+column number prefix is.
  uint64_t StartOfLocationInfo = OS.tell();

  if (!Prefix.empty()) {
    OS << Prefix << ": ";
  }

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

  GetEmitter().GetDiagnosticOutputStream().Flush();
}

diags::TextDiagnosticEmitter::TextDiagnosticEmitter(
    DiagnosticOutputStream &OS, const LangOptions &LangOpts,
    const DiagnosticOptions &DiagOpts)
    : DiagnosticEmitter(OS, LangOpts, DiagOpts) {}

diags::TextDiagnosticEmitter::~TextDiagnosticEmitter() {}
