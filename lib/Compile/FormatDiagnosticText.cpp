#include "stone/Compile/CompilerInvocation.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/ParserResult.h"
using namespace stone;

enum class DiagnosticTextSliceKind {
  None = 0,
  Identifer,
  Percent,
  Select,
  Error,
};

class DiagnosticTextSlice {
  DiagnosticTextSliceKind kind;

protected:
  DiagnosticTextSlice(DiagnosticTextSliceKind kind) : kind(kind) {}
};

// TODO: It would be nice to use the Parser
class DiagnosticTextParser {

  llvm::raw_ostream &Out;

  llvm::ArrayRef<DiagnosticArgument> Args;

  // Keep track of the args we have processed
  unsigned CurArgsIndex = 0;

  Lexer lexer;

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

public:
  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : lexer(BufferID, SM, nullptr, nullptr), Out(Out), Args(Args) {}

  const Token &PeekNext() const { return lexer.Peek(); }

public:
  void Lex(Token &result) { lexer.Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    lexer.Lex(result, leading, trailing);
  }
  bool IsEOF() { return CurTok.GetKind() == tok::eof; }
  bool IsParsing() { return !IsEOF(); }

  SrcLoc Consume() {
    PrevTok = CurTok;
    auto CurLoc = CurTok.GetLoc();
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    Lex(CurTok, LeadingTrivia, TrailingTrivia);
    PrevLoc = CurLoc;
    return CurLoc;
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

public:
  ParserResult<DiagnosticTextSlice> ParseStringLiteralSlice() {}

  ParserResult<DiagnosticTextSlice> ParseIdentifierSlice() {}

  void Parse(llvm::SmallVector<ParserResult<DiagnosticTextSlice>> &slices) {

    if (CurTok.IsLast()) {
      Consume();
    }
    while (IsParsing()) {
      auto slice = ParseTextSlice();
      if (slice.IsNonNull()) {
        slices.push_back(slice);
      }
    }
  }

  ParserResult<DiagnosticTextSlice> ParseTextSlice() {

    ParserResult<DiagnosticTextSlice> slice;

    switch (CurTok.GetKind()) {
    case tok::string_literal: {
      slice = ParseStringLiteralSlice();
      break;
    }
    case tok::identifier: {
      slice = ParseIdentifierSlice();
      break;
    }
    default:
      break;
    }

    return slice;
  }
};

static void ParseDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs, DiagnosticFormatOptions FormatOpts,
    SrcMgr &SM, llvm::SmallVector<ParserResult<DiagnosticTextSlice>> &results) {

  DiagnosticTextParser(SM.addMemBufferCopy(InText), SM, Out, FormatArgs)
      .Parse(results);
}

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,
    DiagnosticFormatOptions FormatOpts) {

  llvm::SmallVector<ParserResult<DiagnosticTextSlice>> results;
  ParseDiagnosticText(Out, InText, FormatArgs, FormatOpts, SM, results);
}
