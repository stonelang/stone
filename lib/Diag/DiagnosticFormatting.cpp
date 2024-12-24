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


  void ParsePercent(){
  	assert(CurTok.IsPercent() && "Expecting percent token");
  	// Send the current text to the output 
    Out << CurTok.GetText();
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
      	ParsePercent();
        break;
      }
      default: {
      	// If we are here, we did not find anything of interest.
      	// So, we consume. 
 		Consume();
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
