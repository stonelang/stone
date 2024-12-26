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
struct DiagnosticTextParser {

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

  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : lexer(BufferID, SM, nullptr, nullptr), Out(Out), Args(Args) {}

  const Token &PeekNext() const { return lexer.Peek(); }

  void Lex(Token &result) { lexer.Lex(result); }
  void Lex(Token &result, llvm::StringRef &leading, llvm::StringRef &trailing) {
    lexer.Lex(result, leading, trailing);
  }

  SrcLoc Consume() {
    PrevTok = CurTok;
    auto CurLoc = CurTok.GetLoc();
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    Lex(CurTok, LeadingTrivia, TrailingTrivia);
    PrevLoc = CurLoc;
    return CurLoc;
  }
  void Parse(ParserResult<DiagnosticTextSlice> &slices) {}

  ParserResult<DiagnosticTextSlice> ParseTextSlice() {

    ParserResult<DiagnosticTextSlice> slice;

    return slice;
  }
};

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,
    DiagnosticFormatOptions FormatOpts) {

  ParserResult<DiagnosticTextSlice> slices;
  DiagnosticTextParser(SM.addMemBufferCopy(InText), SM, Out, FormatArgs).Parse(slices);
}