#include "stone/Compile/CompilerInvocation.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/ParserResult.h"
using namespace stone;

enum class DiagnosticTextSliceKind {
  None = 0,
  Identifer,
  StringLiteral,
  Percent,
  Select,
  Error,
  LBrace,
  RBrace,

};

class DiagnosticTextSlice {
  DiagnosticTextSliceKind kind;
  SrcLoc Loc;

public:
  DiagnosticTextSlice(
      DiagnosticTextSliceKind kind = DiagnosticTextSliceKind::None,
      SrcLoc Loc = SrcLoc())
      : kind(kind), Loc(Loc) {}
  /// Evaluates true when this object stores a diagnostic.
  explicit operator bool() const {
    return (kind != DiagnosticTextSliceKind::None || !Loc.isValid());
  }

public:
  virtual void Format(llvm::raw_ostream &Out) {}
};

class IdentifierTextSlice : public DiagnosticTextSlice {
public:
  IdentifierTextSlice(SrcLoc Loc)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::Identifer, Loc) {}

public:
  void Format(llvm::raw_ostream &Out) override {}
};

class StringLiteralTextSlice : public DiagnosticTextSlice {

public:
  StringLiteralTextSlice(SrcLoc Loc)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::StringLiteral, Loc) {}

public:
  void Format(llvm::raw_ostream &Out) override {}
};

// class SelectTextSlice : public DiagnosticTextSlice {

// public:
//   SelectTextSlice(llvm::StringRef Text) :
//   DiagnosticTextSlice(DiagnosticTextSliceKind::Select, Text) {}
// };

// class ErrorTextSlice : public DiagnosticTextSlice {

// public:
//   ErrorTextSlice(llvm::StringRef Text) :
//   DiagnosticTextSlice(DiagnosticTextSliceKind::Error, Text) {}
// };

// TODO: It would be nice to use the Parser
struct DiagnosticTextSlicer {

  SrcMgr &SM;

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

  DiagnosticTextSlicer(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : lexer(BufferID, SM, nullptr, nullptr), SM(SM), Out(Out), Args(Args) {
    Consume();
  }
  DiagnosticTextSlicer(unsigned BufferID, SrcMgr &SM)
      : lexer(BufferID, SM, nullptr, nullptr), SM(SM), Out(llvm::errs()),
        Args({}) {
    Consume();
  }

  const Token &Peek() const { return lexer.Peek(); }
  void Lex(Token &result) { lexer.Lex(result); }
  bool IsEOF() { return CurTok.GetKind() == tok::eof; }
  bool IsSlicing() { return !IsEOF(); }

  SrcLoc Consume() {
    PrevTok = CurTok;
    auto CurLoc = CurTok.GetLoc();
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    Lex(CurTok);
    PrevLoc = CurLoc;
    return CurLoc;
  }

  // "'%N' "
  DiagnosticTextSlice SliceStringLiteral() {

    assert(CurTok.GetKind() == tok::string_literal &&
           "Expecting string literal");
    return StringLiteralTextSlice(Consume());
  }

  DiagnosticTextSlice SliceIdentifier() {
    assert(CurTok.GetKind() == tok::identifier && "Expecting identifier");
    return IdentifierTextSlice(Consume());
  }

  // "words  %selection{....}N "
  // Slice ParsePercentSlice() {
  //   assert(CurTok.GetKind() == tok::percent && "Expecting a percent");

  //   Slice result;
  //   Consume();
  //   return result;
  // }

  // Slice ParseSelectSlice() {
  //   Slice result;

  //   // assert(CurTok.GetText() == "select" && "requires select modifiers");
  //   // assert(Peek().IsLBrace() && " requires l_brace");
  //   // StartAndEndTracker tracker (CurTok.GetText());

  //   Consume();
  //   return result;
  // }
  unsigned StripIndexFromString(llvm::StringRef CurText) {

    // size_t percentLoc = CurText.find('%');
    // if (percentLoc == StringRef::npos) {

    // }
    // auto separators = CurText.split("%");
    // if(separators.first == "%"){
    //   // Check that the next one is a digit
    // }
  }

  void Slice(llvm::SmallVector<DiagnosticTextSlice> &results) {

    if (CurTok.IsLast()) {
      Consume();
    }
    while (IsSlicing()) {
      auto slice = SliceText();
      if (slice) {
        results.push_back(slice);
      }
    }
  }

  DiagnosticTextSlice SliceText() {
    switch (CurTok.GetKind()) {
    case tok::string_literal: {
      return SliceStringLiteral();
    }
    case tok::identifier: {
      return SliceIdentifier();
    }
    default: {
      Consume();
      break;
    }
    }
    return DiagnosticTextSlice();
  }
};

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,
    DiagnosticFormatOptions FormatOpts) {

  llvm::SmallVector<DiagnosticTextSlice> results;
  DiagnosticTextSlicer(SM.addMemBufferCopy("Hi '%0' and '%1' ok"), SM, Out,
                       FormatArgs)
      .Slice(results);

  /// Merge()
}
