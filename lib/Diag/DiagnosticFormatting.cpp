#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticFormatParser.h"

using namespace stone;

void DiagnosticImpl::FormatDiagnostic(
    llvm::raw_ostream &Out, SrcMgr &SM,
    DiagnosticFormatOptions FormatOpts) const {

  DiagnosticEngine::FormatDiagnosticText(Out, FormatString, SM, FormatArgs,
                                         FormatOpts);
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef Text, SrcMgr &SM,
    DiagnosticFormatOptions FormatOpts) {

  FormatDiagnosticText(Out, Text, SM, {}, FormatOpts);
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText, SrcMgr &SM,
    ArrayRef<DiagnosticArgument> Args, DiagnosticFormatOptions FormatOpts) {

  // unsigned BufferID = SM.addMemBufferCopy(Text);
  // DiagnosticTextParser textParser(BufferID, SM);
  // textParser.Parse();
  // FormatDiagnosticParser().Parse();

  while (!InText.empty()) {
    size_t Percent = InText.find('%');
    if (Percent == StringRef::npos) {
      // Write the rest of the string; we're done.
      Out.write(InText.data(), InText.size());
      break;
    }

    // // Write the string up to (but not including) the %, then drop that text
    // // (including the %).
    // Out.write(InText.data(), Percent);
    // InText = InText.substr(Percent + 1);

    // // '%%' -> '%'.
    // if (InText[0] == '%') {
    //   Out.write('%');
    //   InText = InText.substr(1);
    //   continue;
    // }

    // // Parse an optional modifier.
    // StringRef Modifier;
    // {
    //   size_t Length = InText.find_if_not(isalpha);
    //   Modifier = InText.substr(0, Length);
    //   InText = InText.substr(Length);
    // }

    // if (Modifier == "error") {
    //   Out << StringRef(
    //       "<<INTERNAL ERROR: encountered %error in diagnostic text>>");
    //   continue;
    // }

    // // Parse the optional argument list for a modifier, which is
    // brace-enclosed. StringRef ModifierArguments; if (InText[0] == '{') {
    //   InText = InText.substr(1);
    //   ModifierArguments = skipToDelimiter(InText, '}');
    // }

    // // Find the digit sequence, and parse it into an argument index.
    // size_t Length = InText.find_if_not(isdigit);
    // unsigned ArgIndex;
    // bool IndexParseFailed = InText.substr(0, Length).getAsInteger(10,
    // ArgIndex);

    // if (IndexParseFailed) {
    //   Out << StringRef(
    //       "<<INTERNAL ERROR: unparseable argument index in diagnostic
    //       text>>");
    //   continue;
    // }

    // InText = InText.substr(Length);

    // if (ArgIndex >= Args.size()) {
    //   Out << StringRef(
    //       "<<INTERNAL ERROR: out-of-range argument index in diagnostic
    //       text>>");
    //   continue;
    // }

    // Convert the argument to a string.
    // DiagnosticEngine::FormatDiagnosticArgument(Modifier, ModifierArguments,
    // Args, ArgIndex, FormatOpts, Out);
  }
}
