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

  DiagnosticEngine::FormatDiagnosticText(Out, FormatText, SM, FormatArgs,
                                         FormatOpts);
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef FormatText, SrcMgr &SM,
    ArrayRef<DiagnosticArgument> Args, DiagnosticFormatOptions FormatOpts) {

  DiagnosticEngine::FormatDiagnosticText(
      Out, FormatText.begin(), FormatText.end(), SM, Args, FormatOpts);
}

enum class IntTextKind {
  None = 0,
  Percent,
  LBrace,
  RBrace,
  LParen,
  RParen,
  SingleQuote,
  DoubleQuote,
  WhiteSpace,
  NewLine,
  VerticalTable,
  StringLiteral,
  IntegerLiter,
  Comma,
  Semi,
  Colon,
  Identifier,
  Number,

};

static IntTextKind DetermineInTextKind(const char *CurPtr) {
  switch (*CurPtr++) {
  case '%':
    return IntTextKind::Percent;
  case '{':
    return IntTextKind::LBrace;
  case '}':
    return IntTextKind::RBrace;
  case '(':
    return IntTextKind::LParen;
  case ')':
    return IntTextKind::RParen;
  case '\n':
  case '\r':
    return IntTextKind::NewLine;
  case ' ':
  case '\t':
  case '\f':
  case '\v':
    return IntTextKind::WhiteSpace;
  case ',':
    return IntTextKind::Comma;
  case ';':
    return IntTextKind::Semi;
  case ':':
    return IntTextKind::Colon;
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case '_':
    return IntTextKind::Identifier;
  case '"':
    return IntTextKind::StringLiteral;
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return IntTextKind::Number;
  default:
    return IntTextKind::None;
  }
}

void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, const char *BufferStart, const char *BufferEnd,
    SrcMgr &SM, ArrayRef<DiagnosticArgument> Args,
    DiagnosticFormatOptions FormatOpts) {

  /// Assign the CurPtr to the BufferStart
  const char *CurPtr = BufferStart;
  // Recall the start so we can form the text range.
  const char *TextStart = CurPtr;

  // Keep a count
  unsigned IntTextLoc = 0;

  while (*CurPtr != *BufferEnd) {
    /// May not need.
    //++IntTextLoc;
    auto CurTok = DetermineInTextKind(CurPtr);
    if (CurTok == IntTextKind::None) {
      return;
    }
    // Check for a percent
    if (CurTok == IntTextKind::Percent) {
      // Now, copy the string up to the percent (not including) to the Out
      // buffer
      Out.write(TextStart, static_cast<size_t>(CurPtr - TextStart));
      continue;
    }

    if (CurTok == IntTextKind::Number) {
      if (DetermineInTextKind(CurPtr--) != IntTextKind::Percent) {
        return;
      }
    }
  }
}
