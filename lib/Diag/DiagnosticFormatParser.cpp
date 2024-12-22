#include "stone/Diag/DiagnosticFormatParser.h"
#include "stone/Basic/SrcMgr.h"

using namespace stone;

diags::DiagnosticFormatLexer::DiagnosticFormatLexer(unsigned BufferID,
                                                    llvm::StringRef InText,
                                                    SrcMgr &SM)
    : BufferID(BufferID), InText(InText), SM(SM) {

  auto EndOffset = SM.getRangeForBuffer(BufferID).getByteLength();
  Initialize(/*Offset=*/0, EndOffset);
}

diags::DiagnosticFormatLexer::DiagnosticFormatLexer(llvm::StringRef InText,
                                                    SrcMgr &SM)
    : DiagnosticFormatLexer(SM.addMemBufferCopy(InText), InText, SM) {}

void diags::DiagnosticFormatLexer::Initialize(unsigned Offset,
                                              unsigned EndOffset) {

  assert(Offset <= EndOffset);

  // Initialize buffer pointers.
  StringRef Text = SM.extractText(SM.getRangeForBuffer(BufferID));
  BufferStart = Text.data();
  BufferEnd = Text.data() + Text.size();

  assert(*BufferEnd == 0);
  assert(BufferStart + Offset <= BufferEnd);
  assert(BufferStart + EndOffset <= BufferEnd);

  // Check for Unicode BOM at start of file (Only UTF-8 BOM supported now).
  size_t BOMLength = Text.starts_with("\xEF\xBB\xBF") ? 3 : 0;

  // Keep information about existance of UTF-8 BOM for transparency
  //  source code editing with libSyntax.
  ContentStart = BufferStart + BOMLength;

  // Initialize code completion.
  if (BufferID == SM.getCodeCompletionBufferID()) {
    const char *Ptr = BufferStart + SM.getCodeCompletionOffset();
    if (Ptr >= BufferStart && Ptr <= BufferEnd) {
      CodeCompletionPtr = Ptr;
    }
  }

  ArtificialEOF = BufferStart + EndOffset;
  CurPtr = BufferStart + Offset;

  assert(NextToken.IsInvalid());
  Lex();
  assert((NextToken.AtStartOfLine || CurPtr != BufferStart) &&
         "The token should be at the beginning of the line, "
         "or we should be lexing from the middle of the buffer");
}

void diags::DiagnosticFormatLexer::Lex() {

  assert(CurPtr >= BufferStart && CurPtr <= BufferEnd &&
         "Current pointer out of range!");

  const char *LeadingTriviaStart = CurPtr;

  if (CurPtr == BufferStart) {
    if (BufferStart < ContentStart) {
      size_t BOMLen = ContentStart - BufferStart;
      assert(BOMLen == 3 && "UTF-8 BOM is 3 bytes");
      CurPtr += BOMLen;
    }
    NextToken.AtStartOfLine = true;
  } else {
    NextToken.AtStartOfLine = false;
  }

  // LeadingTrivia = LexTrivia(/*IsForTrailingTrivia=*/false,
  // LeadingTriviaStart);

  // // Remember the start of the token so we can form the text range.
  const char *TokStart = CurPtr;
  if (LexerCutOffPoint && CurPtr >= LexerCutOffPoint) {
    return ConstructToken(DiagnosticFormatTokenKind::eof, TokStart);
  }
  switch (*CurPtr++) {
  default: {
  }
  }
}

void diags::DiagnosticFormatLexer::ConstructToken(
    DiagnosticFormatTokenKind Kind, const char *TokStart) {

  assert(CurPtr >= BufferStart && CurPtr <= BufferEnd &&
         "Current pointer out of range!");

  // // When we are lexing a subrange from the middle of a file buffer, we will
  // // run past the end of the range, but will stay within the file.  Check if
  // // we are past the imaginary EOF, and synthesize a tok::eof in this case.
  if (Kind != DiagnosticFormatTokenKind::eof && TokStart >= ArtificialEOF) {
    Kind = DiagnosticFormatTokenKind::eof;
  }

  StringRef TokenText{TokStart, static_cast<size_t>(CurPtr - TokStart)};

  // if (TriviaRetention == TriviaRetentionMode::WithTrivia && Kind != tok::eof)
  // {
  //   TrailingTrivia = lexTrivia(/*IsForTrailingTrivia=*/true, CurPtr);
  // } else {
  //   TrailingTrivia = StringRef();
  // }
  PrevToken = NextToken;
  NextToken = DiagnosticFormatToken(Kind, TokenText);
}

diags::DiagnosticFormatParser::DiagnosticFormatParser(
    llvm::raw_ostream &Out, llvm::StringRef InText, SrcMgr &SM,
    DiagnosticFormatOptions &FormatOpts)
    : Lexer(InText, SM), Out(Out), FormatOpts(FormatOpts) {}

void diags::DiagnosticFormatParser::Parse() {}

void diags::DiagnosticFormatParser::ParseIdentifier() {}

void diags::DiagnosticFormatParser::ParseSelect() {}
