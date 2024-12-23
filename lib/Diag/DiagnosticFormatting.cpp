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


class DiagnosticTextParser {

  llvm::raw_ostream &Out;

  /// Pointer to the first character of the buffer, even in a lexer that
  /// scans a subrange of the buffer.
  const char *BufferStart;
  /// Pointer to one past the end character of the buffer, even in a lexer
  /// that scans a subrange of the buffer.  Because the buffer is always
  /// NUL-terminated, this points to the NUL terminator.
  const char *BufferEnd;

  /// Pointer to the next not consumed character.
  const char *CurPtr;

  stone::SrcMgr &SM;

  llvm::ArrayRef<DiagnosticArgument> Args;

  bool CutOff;

public:
  DiagnosticTextParser(llvm::raw_ostream &Out, const char *BufferStart,
                       const char *BufferEnd, stone::SrcMgr &SM,
                       ArrayRef<DiagnosticArgument> Args)
      : Out(Out), BufferStart(BufferStart), BufferEnd(BufferEnd), SM(SM),
        Args(Args) {
    CurPtr = BufferStart;
  }

public:
  void Parse() {
    while ((*CurPtr != *BufferEnd) && !CutOff) {
      ParseImpl(CurPtr);
    }
  }

private:
  void ParsePercent(const char *CurPtr) {
    if (*CurPtr != '%') {
      return;
    }
  }
  void ParseLBrace(const char *CurPtr) {
    if (*CurPtr != '{') {
      return;
    }
  }
  void ParseRBrace(const char *CurPtr) {
    if (*CurPtr != '}') {
      return;
    }
  }

  void ParseLParen(const char *CurPtr) {
    if (*CurPtr != '(') {
      return;
    }
  }
  void ParseRParen(const char *CurPtr) {
    if (*CurPtr != '}') {
      return;
    }
  }

  void ParseDigit(const char *CurPtr) {
    if (!clang::isDigit(*CurPtr)) {
    }
  }
  void ParseSelect(const char *CurPtr) {

  	if(*CurPtr != 's'){
  	}

  }
  void ParseImpl(const char *CurPtr) {
    switch (*CurPtr++) {
    case '%': {
      ParsePercent(CurPtr);
      break;
    }
    case '{': {
      ParseLBrace(CurPtr);
      break;
    }
    case '}': {
      ParseRBrace(CurPtr);
      break;
    }
    case '(': {
      ParseLBrace(CurPtr);
      break;
    }
    case ')': {
      ParseRParen(CurPtr);
      break;
    }
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      ParseDigit(CurPtr);
      break;
    }
    case '\n':
    case '\r': {
      // new line should not exist
      CutOff = true;
      break;
    }
    case ' ':
    case '\t':
    case '\f':
    case '\v': {
      // eat
      break;
    }
    case ',':
    	// eat
      break;
    case ';':
    	//eat 
      break;
    case ':':
    	// eat
      break;
    default: {
      if (!clang::isLetter(*CurPtr)) {
        CutOff = true;
      }
      if (*CurPtr == 's') {
        ParseSelect(CurPtr);
      }
      break;
    }
    }
  }
};
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, const char *BufferStart, const char *BufferEnd,
    SrcMgr &SM, ArrayRef<DiagnosticArgument> Args,
    DiagnosticFormatOptions FormatOpts) {
  DiagnosticTextParser(Out, BufferStart, BufferStart, SM, Args).Parse();
}
