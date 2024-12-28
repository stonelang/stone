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
  LBrace,
  RBrace,

};

class DiagnosticTextSlice {
  DiagnosticTextSliceKind kind;

protected:
  DiagnosticTextSlice(DiagnosticTextSliceKind kind) : kind(kind) {}

public:
  virtual void Merge(llvm::raw_ostream &Out) {}
};

class IdentiferTextSlice : public DiagnosticTextSlice {
  llvm::StringRef Text;

public:
  IdentiferTextSlice(llvm::StringRef Text)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::Identifer), Text(Text) {}
};

class PercentTextSlice : public DiagnosticTextSlice {
  unsigned index;

public:
  PercentTextSlice(unsigned index)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::Percent), index(index) {}
};

class SelectTextSlice : public DiagnosticTextSlice {

public:
  SelectTextSlice() : DiagnosticTextSlice(DiagnosticTextSliceKind::Select) {}
};

class ErrorTextSlice : public DiagnosticTextSlice {

public:
  ErrorTextSlice() : DiagnosticTextSlice(DiagnosticTextSliceKind::Error) {}
};

class DiagnosticTextSliceVisitor {};

using Slice = ParserResult<DiagnosticTextSlice>;
using Slices = llvm::SmallVector<Slice>;

// TODO: It would be nice to use the Parser
struct DiagnosticTextParser {

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

  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : lexer(BufferID, SM, nullptr, nullptr), SM(SM), Out(Out), Args(Args) {
    Consume();
  }
  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM)
      : lexer(BufferID, SM, nullptr, nullptr), SM(SM), Out(llvm::errs()),
        Args({}) {
    Consume();
  }

  const Token &PeekNext() const { return lexer.Peek(); }
  void Lex(Token &result) { lexer.Lex(result); }
  bool IsEOF() { return CurTok.GetKind() == tok::eof; }
  bool IsParsing() { return !IsEOF(); }

  SrcLoc Consume() {
    PrevTok = CurTok;
    auto CurLoc = CurTok.GetLoc();
    assert(CurTok.IsNot(tok::eof) && "Lexing past eof!");
    Lex(CurTok);
    PrevLoc = CurLoc;
    return CurLoc;
  }

  // "words '%0' "
  Slice ParseStringLiteralSlice() {
    Slice result;

    // // llvm::StringRef expecting = "'% '"
    // assert(CurTok.GetKind() == tok::string_literal &&
    //        "Expecting string literal");
    // auto CurText = CurTok.GetText();

    // Slices slices;
    // DiagnosticTextParser(SM.addMemBufferCopy(CurTok.GetText()), SM)
    //     .Parse(slices);

    // while (!CurText.empty()) {
    //   if (CurText.find('%') == StringRef::npos) {
    //     return Slice(CurText)
    //     Consume();
    //   }
    //   auto splits = CurText.split('%');
    //   if(!splits.second.empty()){
    //     auto inTextIndex = splits.second.front();
    //   }
    // }

    Consume();
    return result;
  }

  Slice ParseIdentifierSlice() {
    Slice result;

    // if(CurTok.GetText().equals("select")){
    //   return ParseSelectSlice();
    // }
    Consume();
    return result;
  }

  // "words  %selection{....}N "
  Slice ParsePercentSlice() {
    assert(CurTok.GetKind() == tok::percent && "Expecting a percent");

    Slice result;
    Consume();
    return result;
  }

  Slice ParseSelectSlice() {
    Slice result;

    // assert(CurTok.GetText() == "select" && "requires select modifiers");
    // assert(PeekNext().IsLBrace() && " requires l_brace");
    // StartAndEndTracker tracker (CurTok.GetText());

    Consume();
    return result;
  }
  unsigned StripIndexFromString(llvm::StringRef CurText) {

    // size_t percentLoc = CurText.find('%');
    // if (percentLoc == StringRef::npos) {

    // }
    // auto separators = CurText.split("%");
    // if(separators.first == "%"){
    //   // Check that the next one is a digit
    // }
  }

  void Parse(Slices &slices) {

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

  Slice ParseTextSlice() {
    Slice slice;
    switch (CurTok.GetKind()) {
    case tok::string_literal: {
      slice = ParseStringLiteralSlice();
      break;
    }
    case tok::identifier: {
      slice = ParseIdentifierSlice();
      break;
    }
    case tok::percent: {
      slice = ParsePercentSlice();
      break;
    }
    default: {
      Consume();
      break;
    }
    }
    return slice;
  }
};

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,
    DiagnosticFormatOptions FormatOpts) {

  Slices results;
  DiagnosticTextParser(SM.addMemBufferCopy("Hi '%0' and'%1' and '%2' ok"), SM,
                       Out, FormatArgs)
      .Parse(results);

  /// Merge()
}
