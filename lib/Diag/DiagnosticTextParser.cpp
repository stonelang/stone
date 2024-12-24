#include "stone/Diag/DiagnosticClient.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DiagnosticText.h"

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

struct DiagnosticTextParser final {
  llvm::raw_ostream &Out;

  llvm::ArrayRef<DiagnosticArgument> Args;

  // Keep track of the args we have processed
  unsigned ArgsIndex = 0;

  stone::diag::DiagnosticTextLexer Lexer;

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

  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : Lexer(BufferID, SM, llvm::errs()), Out(Out), Args(Args) {}

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

  void ParsePercent() {
    assert(PrevTok.IsTick() && " Expecting previous token to be a tick");
    assert(CurTok.IsPercent() && "Expecting percent token");
    // Send the current text to the output
    // Out << CurTok.GetText();

    llvm::outs() << "here";

    /// assert(CurTok.PeekNext().IsIntegerLiteral() && " ")

    // assert(clang::isDigit(CurTok.PeekNext().GetText())
  }

  void ParseLParen() {
    assert(CurTok.GetKind() == tok::l_paren && "Expecting left paren");
    // Now, we can consume the token ... what follows is a type
    Consume();

    // if (CurTok.IsIdentifer()) {
    //   // This is StringRef, or some builtin type
    // }

    switch (CurTok.GetKind()) {

    case tok::kw_bool: {
    }
    }
    // Send the current text to the output
    Out << CurTok.GetText();
  }

  void Parse() {

    if (CurTok.IsLast()) {
      Consume();
    }
    while (IsParsing()) {
      switch (CurTok.GetKind()) {

      case tok::tick: {
        Consume();
        break;
      }
      case tok::percent: {
        ParsePercent();
        break;
      }
      case tok::l_paren: {
        ParseLParen();
        break;
      }
      case tok::identifier: {
        Consume();
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

void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &Out, SrcMgr &SM, StringRef Text,
    ArrayRef<DiagnosticArgument> Args, DiagnosticFormatOptions FormatOpts) {

  DiagnosticTextParser(SM.addMemBufferCopy(Text), SM, Out, Args).Parse();
}

void DiagnosticEngine::FormatDiagnosticText(
    llvm::raw_ostream &OS, SrcMgr &SM, const DiagnosticInfo &DI,
    DiagnosticFormatOptions FormatOpts) {

  if (!DI) {
    return;
  }
  DiagnosticEngine::FormatDiagnosticText(OS, SM, DI.FormatText, DI.FormatArgs,
                                         FormatOpts);
}
