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

enum class DiagnosticTextToken {
  None = 0,
  Percent,
  LBrace,
  RBrace,
  LParen,
  RParen,
};

static DiagnosticTextToken DetermineDiagnosticTextToken(const char *CharPtr) {

  switch (*CharPtr) {
  case '%':
    return DiagnosticTextToken::Percent;
  case '{':
    return DiagnosticTextToken::Percent;
  case '}':
    return DiagnosticTextToken::Percent;
  case '(':
    return DiagnosticTextToken::Percent;
  case ')':
    return DiagnosticTextToken::Percent;
  default:
    return DiagnosticTextToken::None;
  }
}
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, const char *StrPtr, const char *EndPtr, SrcMgr &SM,
    ArrayRef<DiagnosticArgument> Args, DiagnosticFormatOptions FormatOpts) {

  while (*StrPtr != *EndPtr) {

    switch (*StrPtr) {
    case '%': {
    }
    }
  }
}
