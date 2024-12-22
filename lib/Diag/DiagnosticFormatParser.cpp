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

  case '{':
    return ConstructToken(DiagnosticFormatTokenKind::LBrace, TokStart);
  case '[':
    return ConstructToken(DiagnosticFormatTokenKind::LSquare, TokStart);
  case '(':
    return ConstructToken(DiagnosticFormatTokenKind::LParen, TokStart);
  case '}':
    return ConstructToken(DiagnosticFormatTokenKind::RBrace, TokStart);
  case ']':
    return ConstructToken(DiagnosticFormatTokenKind::RSquare, TokStart);
  case ')':
    return ConstructToken(DiagnosticFormatTokenKind::RParen, TokStart);
  case ',':
    return ConstructToken(DiagnosticFormatTokenKind::Comma, TokStart);
  case ';':
    return ConstructToken(DiagnosticFormatTokenKind::Semi, TokStart);
  case '%':
    return ConstructToken(DiagnosticFormatTokenKind::Percent, TokStart);
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'G':
  case 'H':
  case 'I':
  case 'J':
  case 'K':
  case 'L':
  case 'M':
  case 'N':
  case 'O':
  case 'P':
  case 'Q':
  case 'R':
  case 'S':
  case 'T':
  case 'U':
  case 'V':
  case 'W':
  case 'X':
  case 'Y':
  case 'Z':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
  case 'g':
  case 'h':
  case 'i':
  case 'j':
  case 'k':
  case 'l':
  case 'm':
  case 'n':
  case 'o':
  case 'p':
  case 'q':
  case 'r':
  case 's':
  case 't':
  case 'u':
  case 'v':
  case 'w':
  case 'x':
  case 'y':
  case 'z':
  case '_':
    return LexIdentifier();
  default: {
  }
  }
}
void diags::DiagnosticFormatLexer::LexIdentifier() {}
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

void diags::DiagnosticFormatParser::Parse() {

  while (CurTok.Kind != DiagnosticFormatTokenKind::eof) {
    Lex();
    switch (CurTok.Kind) {
    case DiagnosticFormatTokenKind::Percent:
      break;
    case DiagnosticFormatTokenKind::LBrace:
      break;
    case DiagnosticFormatTokenKind::RBrace:
      break;
    case DiagnosticFormatTokenKind::Identifier:
      break;
    case DiagnosticFormatTokenKind::LSquare:
      return;
    case DiagnosticFormatTokenKind::RSquare:
      return;
    default: {
      // Not aware of this token ...
    }
    }
  }
}
void diags::DiagnosticFormatParser::Lex() {}

void diags::DiagnosticFormatParser::ParseIdentifier() {}

void diags::DiagnosticFormatParser::ParseSelect() {}
