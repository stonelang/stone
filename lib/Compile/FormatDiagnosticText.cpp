#include "stone/Compile/CompilerInvocation.h"
#include "stone/Parse/Lexer.h"
#include "stone/Parse/ParserResult.h"
using namespace stone;

template <typename AlignTy> class SliceAllocation {
public:
  /// Disable non-placement new.
  void *operator new(size_t) = delete;
  void *operator new[](size_t) = delete;

  /// Disable non-placement delete.
  void operator delete(void *) = delete;
  void operator delete[](void *) = delete;

  /// Custom version of 'new' that uses the SILModule's BumpPtrAllocator with
  /// precise alignment knowledge.  This is templated on the allocator type so
  /// that this doesn't require including SILModule.h.
  template <typename ContextTy>
  void *operator new(size_t Bytes, const ContextTy &C,
                     size_t Alignment = alignof(AlignTy)) {
    return C.Allocate(Bytes, Alignment);
  }
};
enum class DiagnosticTextSliceKind {
  None = 0,
  Identifer,
  Percent,
  Select,
  Error,
};

class DiagnosticTextSlice : public SliceAllocation<DiagnosticTextSlice> {
  DiagnosticTextSliceKind kind;

protected:
  DiagnosticTextSlice(DiagnosticTextSliceKind kind) : kind(kind) {}

public:
  virtual void Merge() {}
};

class IdentiferTextSlice : public DiagnosticTextSlice {

public:
  IdentiferTextSlice(llvm::StringRef Text)
      : DiagnosticTextSlice(DiagnosticTextSliceKind::Identifer) {}
};

class PercentTextSlice : public DiagnosticTextSlice {

public:
  PercentTextSlice() : DiagnosticTextSlice(DiagnosticTextSliceKind::Percent) {}
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

  /// Slices allocator
  mutable llvm::BumpPtrAllocator allocator;

public:
  DiagnosticTextParser(unsigned BufferID, SrcMgr &SM, llvm::raw_ostream &Out,
                       ArrayRef<DiagnosticArgument> Args)
      : lexer(BufferID, SM, nullptr, nullptr), Out(Out), Args(Args) {}

  void *Allocate(unsigned long Bytes, unsigned Alignment = 8) const {
    if (Bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(Bytes, Alignment);
  }

public:
  const Token &PeekNext() const { return lexer.Peek(); }
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
  Slice ParseStringLiteralSlice() {
    Slice result;
    return result;
  }

  Slice ParseIdentifierSlice() {
    Slice result;
    return result;
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
    default:
      break;
    }

    return slice;
  }
};

static void ParseDiagnosticText(llvm::raw_ostream &Out, StringRef InText,
                                ArrayRef<DiagnosticArgument> FormatArgs,
                                DiagnosticFormatOptions FormatOpts, SrcMgr &SM,
                                Slices &results) {

  DiagnosticTextParser(SM.addMemBufferCopy(InText), SM, Out, FormatArgs)
      .Parse(results);
}

void CompilerDiagnosticFormatter::FormatDiagnosticText(
    llvm::raw_ostream &Out, StringRef InText,
    ArrayRef<DiagnosticArgument> FormatArgs,
    DiagnosticFormatOptions FormatOpts) {

  Slices results;
  ParseDiagnosticText(Out, InText, FormatArgs, FormatOpts, SM, results);
}
