#include "stone/Basic/Allocation.h"
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

class DiagnosticTextSlice : public Allocation<DiagnosticTextSlice> {

protected:
  DiagnosticTextSliceKind kind;
  llvm::StringRef Text;

public:
  DiagnosticTextSlice(
      DiagnosticTextSliceKind kind = DiagnosticTextSliceKind::None,
      llvm::StringRef Text = llvm::StringRef())
      : kind(kind), Text(Text) {}

  /// Evaluates true when this object stores a diagnostic.
  explicit operator bool() const {
    return (kind != DiagnosticTextSliceKind::None && !Text.empty());
  }

public:
  // Format the text
  virtual void Format(llvm::raw_ostream &Out,
                      const DiagnosticArgument *Arg = nullptr) {}

  // Get the text
  llvm::StringRef GetText() { return Text; }

  ///\return slice kind
  DiagnosticTextSliceKind GetKind() { return kind; }
};

class IdentifierTextSlice : public DiagnosticTextSlice {
public:
  IdentifierTextSlice(llvm::StringRef Text)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::Identifer, Text) {}

public:
  void Format(llvm::raw_ostream &Out,
              const DiagnosticArgument *Arg = nullptr) override {
    Out << " " << Text;
  }
};

class StringLiteralTextSlice : public DiagnosticTextSlice {

  bool PercentFound = false;
  unsigned PercentIndex;

public:
  StringLiteralTextSlice(llvm::StringRef Text)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::StringLiteral, Text) {}

public:
  void Format(llvm::raw_ostream &Out,
              const DiagnosticArgument *Arg = nullptr) override {

    size_t PercentLoc = Text.find('%');

    Out << " two ";
  }

  unsigned GetPercentIndex() { return PercentIndex; }
  bool HasPercentIndex() { return PercentFound != false; }
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

  DiagnosticEngine &DE;

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

  DiagnosticTextSlicer(unsigned BufferID, SrcMgr &SM, DiagnosticEngine &DE,
                       llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : lexer(BufferID, SM, nullptr, nullptr), SM(SM), DE(DE), Out(Out),
        Args(Args) {
    Consume();
  }
  DiagnosticTextSlicer(unsigned BufferID, SrcMgr &SM, DiagnosticEngine &DE)
      : lexer(BufferID, SM, nullptr, nullptr), SM(SM), DE(DE),
        Out(llvm::errs()), Args({}) {
    Consume();
  }

  const Token &Peek() const { return lexer.Peek(); }
  void Lex(Token &result) { lexer.Lex(result); }
  bool IsEOF() { return CurTok.GetKind() == tok::eof; }
  bool IsSlicing() { return !IsEOF(); }

  SrcLoc ConsumeImpl() {
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    auto CurLoc = CurTok.GetLoc();
    PrevTok = CurTok;
    Lex(CurTok);
    return CurLoc;
  }

  SrcLoc Consume() {
    auto CurLoc = ConsumeImpl();
    PrevLoc = CurLoc;
    return CurLoc;
  }

  // "'%N' "
  ParserResult<DiagnosticTextSlice> SliceStringLiteral() {

    assert(CurTok.GetKind() == tok::string_literal &&
           "Expecting string literal");
    auto CurText = CurTok.GetText();
    Consume();
    return stone::MakeParserResult<StringLiteralTextSlice>(
        new (DE) StringLiteralTextSlice(CurText));
  }

  ParserResult<DiagnosticTextSlice> SliceIdentifier() {
    assert(CurTok.GetKind() == tok::identifier && "Expecting identifier");

    auto CurText = CurTok.GetText();
    Consume();
    return stone::MakeParserResult<IdentifierTextSlice>(
        new (DE) IdentifierTextSlice(CurText));
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

  void Slice(llvm::SmallVector<ParserResult<DiagnosticTextSlice>> &results) {

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

  ParserResult<DiagnosticTextSlice> SliceText() {
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
    return stone::ParserResult<DiagnosticTextSlice>();
  }
};

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,DiagnosticEngine& DE,
    DiagnosticFormatOptions FormatOpts) {

  llvm::raw_ostream &console = llvm::errs();

  llvm::SmallVector<ParserResult<DiagnosticTextSlice>> results;
  DiagnosticTextSlicer(SM.addMemBufferCopy("Hi, '%0' is ok"), SM, DE, console,
                       FormatArgs)
      .Slice(results);

  for (auto slice : results) {
    if(slice){
      slice.Get()->Format(console);
    }
    //slice.Format(console);
  }
  console.flush();

  /// Merge()
}
