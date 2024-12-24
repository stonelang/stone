#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticFormatLexer.h"

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

class DiagnosticFormatParser {
  llvm::raw_ostream &Out;
  stone::diag::DiagnosticFormatLexer Lexer;

  // This is the previous token pasrsed by the parser.
  Token PrevTok;

  /// This is the current token being considered by the parser.
  Token CurTok;

  /// The location of the previous tok.
  SrcLoc PrevLoc;

  /// Leading trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef LeadingTrivia;

  /// Trailing trivia for \c Tok.
  /// Always empty if !SF.shouldBuildSyntaxTree().
  llvm::StringRef TrailingTrivia;

  bool IsEOF() { return CurTok.GetKind() == tok::eof; }
  bool IsParsing() { return !IsEOF(); }

  void Lex(Token &result) { Lexer.Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    Lexer.Lex(result, leading, trailing);
  }

  SrcLoc Consume(tok kind) {
    assert(CurTok.Is(kind) && "Consuming wrong curTok type");
    return Consume();
  }

  /// If the current curTok is the collectorified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind) {
    if (CurTok.IsNot(kind)) {
      return false;
    }
    Consume(kind);
    return true;
  }
  /// If the current curTok is the collectorified kind, consume it and
  /// return true.  Otherwise, return false without consuming it.
  bool ConsumeIf(tok kind, SrcLoc &consumedLoc) {
    if (CurTok.IsNot(kind)) {
      return false;
    }
    consumedLoc = Consume(kind);
    return true;
  }
  const Token &PeekNext() const { return Lexer.Peek(); }

  SrcLoc Consume() {
    PrevTok = CurTok;
    auto CurLoc = CurTok.GetLoc();
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    Lex(CurTok, LeadingTrivia, TrailingTrivia);
    PrevLoc = CurLoc;
    return CurLoc;
  }

public:
  DiagnosticFormatParser(unsigned BufferID, stone::SrcMgr &SM,
                         llvm::raw_ostream &Diag)
      : Lexer(BufferID, SM, llvm::errs()), Out(Out) {}

public:
  void Parse() {
    if (CurTok.IsLast()) {
      Consume();
    }
    while (IsParsing()) {

      switch (CurTok.GetKind()) {
      case tok::percent: {
        break;
      }
      default: {
      }
      }
    }
  }
};

void DiagnosticImpl::FormatDiagnostic(
    llvm::raw_ostream &Out, SrcMgr &SM,
    DiagnosticFormatOptions FormatOpts) const {

  DiagnosticEngine::FormatDiagnosticText(Out, FormatText, SM, FormatArgs,
                                         FormatOpts);
}

/// Format the given diagnostic text and place the result in the given
/// buffer.
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef FormatText, stone::SrcMgr &SM,
    ArrayRef<DiagnosticArgument> Args, DiagnosticFormatOptions FormatOpts) {

  auto BufferID = SM.addMemBufferCopy(FormatText);
  DiagnosticFormatParser(BufferID, SM, Out).Parse();
}

// class DiagnosticTextParser {

//   llvm::raw_ostream &Out;

//   /// Pointer to the first character of the buffer, even in a lexer that
//   /// scans a subrange of the buffer.
//   const char *BufferStart;
//   /// Pointer to one past the end character of the buffer, even in a lexer
//   /// that scans a subrange of the buffer.  Because the buffer is always
//   /// NUL-terminated, this points to the NUL terminator.
//   const char *BufferEnd;

//   /// Pointer to the next not consumed character.
//   const char *CurPtr;

//   stone::SrcMgr &SM;

//   llvm::ArrayRef<DiagnosticArgument> Args;

//   bool CutOff;

// public:
//   DiagnosticTextParser(llvm::raw_ostream &Out, const char *BufferStart,
//                        const char *BufferEnd, stone::SrcMgr &SM,
//                        ArrayRef<DiagnosticArgument> Args)
//       : Out(Out), BufferStart(BufferStart), BufferEnd(BufferEnd), SM(SM),
//         Args(Args) {
//     CurPtr = BufferStart;
//   }

// public:
//   void Parse() {
//     while ((*CurPtr != *BufferEnd) && !CutOff) {
//       ParseImpl(CurPtr);
//     }
//   }

// private:
//   void ParsePercent(const char *CurPtr) {
//     if (*CurPtr != '%') {
//       return;
//     }
//   }
//   void ParseLBrace(const char *CurPtr) {
//     if (*CurPtr != '{') {
//       return;
//     }
//   }
//   void ParseRBrace(const char *CurPtr) {
//     if (*CurPtr != '}') {
//       return;
//     }
//   }

//   void ParseLParen(const char *CurPtr) {
//     if (*CurPtr != '(') {
//       return;
//     }
//   }
//   void ParseRParen(const char *CurPtr) {
//     if (*CurPtr != '}') {
//       return;
//     }
//   }

//   void ParseDigit(const char *CurPtr) {
//     if (!clang::isDigit(*CurPtr)) {
//     }
//   }
//   void ParseSelect(const char *CurPtr) {

//     if (*CurPtr != 's') {
//     }
//   }
//   void ParseImpl(const char *CurPtr) {
//     switch (*CurPtr++) {
//     case '%': {
//       ParsePercent(CurPtr);
//       break;
//     }
//     case '{': {
//       ParseLBrace(CurPtr);
//       break;
//     }
//     case '}': {
//       ParseRBrace(CurPtr);
//       break;
//     }
//     case '(': {
//       ParseLBrace(CurPtr);
//       break;
//     }
//     case ')': {
//       ParseRParen(CurPtr);
//       break;
//     }
//     case '0':
//     case '1':
//     case '2':
//     case '3':
//     case '4':
//     case '5':
//     case '6':
//     case '7':
//     case '8':
//     case '9': {
//       ParseDigit(CurPtr);
//       break;
//     }
//     case '\n':
//     case '\r': {
//       // new line should not exist
//       CutOff = true;
//       break;
//     }
//     case ' ':
//     case '\t':
//     case '\f':
//     case '\v': {
//       // eat
//       break;
//     }
//     case ',':
//       // eat
//       break;
//     case ';':
//       // eat
//       break;
//     case ':':
//       // eat
//       break;
//     default: {
//       if (!clang::isLetter(*CurPtr)) {
//         CutOff = true;
//       }
//       if (*CurPtr == 's') {
//         ParseSelect(CurPtr);
//       }
//       break;
//     }
//     }
//   }
// };
void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, const char *BufferStart, const char *BufferEnd,
    SrcMgr &SM, ArrayRef<DiagnosticArgument> Args,
    DiagnosticFormatOptions FormatOpts) {

  // DiagnosticTextParser(Out, BufferStart, BufferStart, SM, Args).Parse();
}
