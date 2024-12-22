#include "stone/Diag/DiagnosticTextParser.h"
#include "stone/Basic/SrcMgr.h"

using namespace stone;

diags::DiagnosticTextParser::DiagnosticTextParser(unsigned BufferID, SrcMgr &SM)
    : BufferID(BufferID), SM(SM) {

  unsigned EndOffset = SM.getRangeForBuffer(BufferID).getByteLength();

  Initialize(/*Offset=*/0, EndOffset);
}

void diags::DiagnosticTextParser::Initialize(unsigned Offset,
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

  // Keep information about existance of UTF-8 BOM for transparency source code
  // editing with libSyntax.
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

  // assert(NextToken.Is(tok::LAST));
  Lex();
  // assert((NextToken.IsAtStartOfLine() || CurPtr != BufferStart) &&
  //        "The token should be at the beginning of the line, "
  //        "or we should be lexing from the middle of the buffer");
}

void diags::DiagnosticTextParser::Lex() {

  assert(CurPtr >= BufferStart && CurPtr <= BufferEnd &&
         "Current pointer out of range!");

  const char *LeadingTriviaStart = CurPtr;

  if (CurPtr == BufferStart) {
    if (BufferStart < ContentStart) {
      size_t BOMLen = ContentStart - BufferStart;
      assert(BOMLen == 3 && "UTF-8 BOM is 3 bytes");
      CurPtr += BOMLen;
    }
    NextToken.SetAtStartOfLine(true);
  } else {
    NextToken.SetAtStartOfLine(false);
  }

  // LeadingTrivia = lexTrivia(/*IsForTrailingTrivia=*/false,
  // LeadingTriviaStart);

  // // Remember the start of the token so we can form the text range.
  const char *TokStart = CurPtr;

  // if (LexerCutOffPoint && CurPtr >= LexerCutOffPoint) {
  //   return formToken(TextTokenKind::eof, TokStart);
  // }

  switch (*CurPtr++) {
  default: {
  }
  }
}

void diags::DiagnosticTextParser::Parse() {}

void diags::DiagnosticTextParser::ParseIdentifier() {}

void diags::DiagnosticTextParser::ParseSelect() {}
