#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "clang/Basic/CharInfo.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/ConvertUTF.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Unicode.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <utility>
#include <vector>

using namespace stone;
using namespace clang;

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

  while (!InText.empty()) {
    // Find the location of the percent symbol
    size_t Percent = InText.find('%');
    if (Percent == StringRef::npos) {
      // Write the rest of the string; we're done.
      Out.write(InText.data(), InText.size());
      break;
    }

    // Write the string up to but not including the % into Out.
    Out.write(InText.data(), Percent);

    // Remove the text up to and include the % from InText.
    InText = InText.substr(Percent + 1);

    // It is possible that you have '%%' -- we treat it as '%'.
    if (InText[0] == '%') {
      // Write percent to Out
      Out.write('%');
      // Remove the extra percent
      InText = InText.substr(1);
      continue;
    }
  }
}

// void DiagnosticEngine::FormatDiagnosticText(
//     llvm::raw_ostream &Out, const char *DiagStr, const char *DiagEnd,
//     SrcMgr &SM, ArrayRef<DiagnosticArgument> Args,
//     DiagnosticFormatOptions FormatOpts) {

//   while (DiagStr != DiagEnd) {

//     // if (*DiagStr != '%') {
//     //   // Append non-%0 substrings to Str if we have one.
//     //   const char *StrEnd = std::find(DiagStr, DiagEnd, '%');
//     //   Out.write(DiagStr, StrEnd);
//     //   DiagStr = StrEnd;
//     //   continue;
//     // }
//     // else if (isPunctuation((*DiagStr)+1)) {
//     //   Out.write(DiagStr[1]);  // %% -> %.
//     //   DiagStr += 2;
//     //   continue;
//     // }
//   }
// }
