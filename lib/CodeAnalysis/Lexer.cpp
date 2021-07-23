#include "stone/Basic/Char.h"
#include "stone/Basic/SrcMgr.h"
#include "stone/Basic/SyntaxDiagnostic.h"
#include "stone/CodeAnalysis/Lexer.h"
#include "stone/CodeAnalysis/LexerPipeline.h"

using namespace stone;
using namespace stone::syn;

static bool EncodeToUTF8(unsigned CharValue, SmallVectorImpl<char> &Result) {
  // Number of bits in the value, ignoring leading zeros.
  unsigned NumBits = 32 - llvm::countLeadingZeros(CharValue);

  // Handle the leading byte, based on the number of bits in the value.
  unsigned NumTrailingBytes;
  if (NumBits <= 5 + 6) {
    // Encoding is 0x110aaaaa 10bbbbbb
    Result.push_back(char(0xC0 | (CharValue >> 6)));
    NumTrailingBytes = 1;
  } else if (NumBits <= 4 + 6 + 6) {
    // Encoding is 0x1110aaaa 10bbbbbb 10cccccc
    Result.push_back(char(0xE0 | (CharValue >> (6 + 6))));
    NumTrailingBytes = 2;

    // UTF-16 surrogate pair values are not valid code points.
    if (CharValue >= 0xD800 && CharValue <= 0xDFFF)
      return true;
    // U+FDD0...U+FDEF are also reserved
    if (CharValue >= 0xFDD0 && CharValue <= 0xFDEF)
      return true;
  } else if (NumBits <= 3 + 6 + 6 + 6) {
    // Encoding is 0x11110aaa 10bbbbbb 10cccccc 10dddddd
    Result.push_back(char(0xF0 | (CharValue >> (6 + 6 + 6))));
    NumTrailingBytes = 3;
    // Reject over-large code points.  These cannot be encoded as UTF-16
    // surrogate pairs, so UTF-32 doesn't allow them.
    if (CharValue > 0x10FFFF)
      return true;
  } else {
    return true; // UTF8 can encode these, but they aren't valid code points.
  }

  // Emit all of the trailing bytes.
  while (NumTrailingBytes--)
    Result.push_back(
        char(0x80 | (0x3F & (CharValue >> (NumTrailingBytes * 6)))));
  return false;
}

/// CLO8 - Return the number of leading ones in the specified 8-bit value.
static unsigned CLO8(unsigned char C) {
  return llvm::countLeadingOnes(uint32_t(C) << 24);
}

/// isStartOfUTF8Character - Return true if this isn't a UTF8 continuation
/// character, which will be of the form 0b10XXXXXX
static bool IsStartOfUTF8Char(unsigned char C) {
  // RFC 2279: The octet values FE and FF never appear.
  // RFC 3629: The octet values C0, C1, F5 to FF never appear.
  return C <= 0x80 || (C >= 0xC2 && C < 0xF5);
}

/// validateUTF8CharacterAndAdvance - Given a pointer to the starting byte of a
/// UTF8 character, validate it and advance the lexer past it.  This returns the
/// encoded character or ~0U if the encoding is invalid.
uint32_t syn::ValidateUTF8CharAndAdvance(const char *&Ptr, const char *End) {
  if (Ptr >= End)
    return ~0U;

  unsigned char CurByte = *Ptr++;
  if (CurByte < 0x80)
    return CurByte;

  // Read the number of high bits set, which indicates the number of bytes in
  // the character.
  unsigned EncodedBytes = CLO8(CurByte);

  // If this is 0b10XXXXXX, then it is a continuation character.
  if (EncodedBytes == 1 || !IsStartOfUTF8Char(CurByte)) {
    // Skip until we get the start of another character.  This is guaranteed to
    // at least stop at the nul at the end of the buffer.
    while (Ptr < End && !IsStartOfUTF8Char(*Ptr))
      ++Ptr;
    return ~0U;
  }

  // Drop the high bits indicating the # bytes of the result.
  unsigned CharValue = (unsigned char)(CurByte << EncodedBytes) >> EncodedBytes;

  // Read and validate the continuation bytes.
  for (unsigned i = 1; i != EncodedBytes; ++i) {
    if (Ptr >= End)
      return ~0U;
    CurByte = *Ptr;
    // If the high bit isn't set or the second bit isn't clear, then this is not
    // a continuation byte!
    if (CurByte < 0x80 || CurByte >= 0xC0)
      return ~0U;

    // Accumulate our result.
    CharValue <<= 6;
    CharValue |= CurByte & 0x3F;
    ++Ptr;
  }

  // UTF-16 surrogate pair values are not valid code points.
  if (CharValue >= 0xD800 && CharValue <= 0xDFFF)
    return ~0U;

  // If we got here, we read the appropriate number of accumulated bytes.
  // Verify that the encoding was actually minimal.
  // Number of bits in the value, ignoring leading zeros.
  unsigned NumBits = 32 - llvm::countLeadingZeros(CharValue);

  if (NumBits <= 5 + 6)
    return EncodedBytes == 2 ? CharValue : ~0U;
  if (NumBits <= 4 + 6 + 6)
    return EncodedBytes == 3 ? CharValue : ~0U;
  return EncodedBytes == 4 ? CharValue : ~0U;
}

static bool IsValidIdentifierContinuationCodePoint(uint32_t c) {
  if (c < 0x80)
    return stone::isIdentifierBody(c, /*dollar*/ true);

  // N1518: Recommendations for extended identifier characters for C and C++
  // Proposed Annex X.1: Ranges of characters allowed
  return c == 0x00A8 || c == 0x00AA || c == 0x00AD || c == 0x00AF ||
         (c >= 0x00B2 && c <= 0x00B5) || (c >= 0x00B7 && c <= 0x00BA) ||
         (c >= 0x00BC && c <= 0x00BE) || (c >= 0x00C0 && c <= 0x00D6) ||
         (c >= 0x00D8 && c <= 0x00F6) || (c >= 0x00F8 && c <= 0x00FF)

         || (c >= 0x0100 && c <= 0x167F) || (c >= 0x1681 && c <= 0x180D) ||
         (c >= 0x180F && c <= 0x1FFF)

         || (c >= 0x200B && c <= 0x200D) || (c >= 0x202A && c <= 0x202E) ||
         (c >= 0x203F && c <= 0x2040) || c == 0x2054 ||
         (c >= 0x2060 && c <= 0x206F)

         || (c >= 0x2070 && c <= 0x218F) || (c >= 0x2460 && c <= 0x24FF) ||
         (c >= 0x2776 && c <= 0x2793) || (c >= 0x2C00 && c <= 0x2DFF) ||
         (c >= 0x2E80 && c <= 0x2FFF)

         || (c >= 0x3004 && c <= 0x3007) || (c >= 0x3021 && c <= 0x302F) ||
         (c >= 0x3031 && c <= 0x303F)

         || (c >= 0x3040 && c <= 0xD7FF)

         || (c >= 0xF900 && c <= 0xFD3D) || (c >= 0xFD40 && c <= 0xFDCF) ||
         (c >= 0xFDF0 && c <= 0xFE44) || (c >= 0xFE47 && c <= 0xFFF8)

         || (c >= 0x10000 && c <= 0x1FFFD) || (c >= 0x20000 && c <= 0x2FFFD) ||
         (c >= 0x30000 && c <= 0x3FFFD) || (c >= 0x40000 && c <= 0x4FFFD) ||
         (c >= 0x50000 && c <= 0x5FFFD) || (c >= 0x60000 && c <= 0x6FFFD) ||
         (c >= 0x70000 && c <= 0x7FFFD) || (c >= 0x80000 && c <= 0x8FFFD) ||
         (c >= 0x90000 && c <= 0x9FFFD) || (c >= 0xA0000 && c <= 0xAFFFD) ||
         (c >= 0xB0000 && c <= 0xBFFFD) || (c >= 0xC0000 && c <= 0xCFFFD) ||
         (c >= 0xD0000 && c <= 0xDFFFD) || (c >= 0xE0000 && c <= 0xEFFFD);
}
static bool IsValidIdentifierStartCodePoint(uint32_t c) {
  if (!IsValidIdentifierContinuationCodePoint(c))
    return false;

  if (c < 0x80 && (stone::isDigit(c) || c == '$'))
    return false;

  // N1518: Recommendations for extended identifier characters for C and C++
  // Proposed Annex X.2: Ranges of characters disallowed initially
  if ((c >= 0x0300 && c <= 0x036F) || (c >= 0x1DC0 && c <= 0x1DFF) ||
      (c >= 0x20D0 && c <= 0x20FF) || (c >= 0xFE20 && c <= 0xFE2F))
    return false;

  return true;
}
static bool AdvanceIf(char const *&ptr, char const *end,
                      bool (*predicate)(uint32_t)) {
  char const *next = ptr;
  uint32_t c = ValidateUTF8CharAndAdvance(next, end);
  if (c == ~0U)
    return false;
  if (predicate(c)) {
    ptr = next;
    return true;
  }
  return false;
}

static bool AdvanceIfValidStartOfIdentifier(char const *&ptr, char const *end) {
  return AdvanceIf(ptr, end, IsValidIdentifierStartCodePoint);
}
static bool AdvanceIfValidContinuationOfIdentifier(char const *&ptr,
                                                   char const *end) {
  return AdvanceIf(ptr, end, IsValidIdentifierContinuationCodePoint);
}

/// Is the operator beginning at the given character "left-bound"?
static bool IsLeftBound(const char *tokBegin, const char *bufferBegin) {
  // The first character in the file is not left-bound.
  if (tokBegin == bufferBegin)
    return false;

  switch (tokBegin[-1]) {
  case ' ':
  case '\r':
  case '\n':
  case '\t': // whitespace
  case '(':
  case '[':
  case '{': // opening delimiters
  case ',':
  case ';':
  case ':':  // expression separators
  case '\0': // whitespace / last char in file
    return false;

  case '/':
    if (tokBegin - 1 != bufferBegin && tokBegin[-2] == '*')
      return false; // End of a slash-star comment, so whitespace.
    else
      return true;

  case '\xA0':
    if (tokBegin - 1 != bufferBegin && tokBegin[-2] == '\xC2')
      return false; // Non-breaking whitespace (U+00A0)
    else
      return true;

  default:
    return true;
  }
}

/// Is the operator ending at the given character (actually one past the end)
/// "right-bound"?
///
/// The code-completion point is considered right-bound.
static bool IsRightBound(const char *tokEnd, bool isLeftBound,
                         const char *codeCompletionPtr) {
  switch (*tokEnd) {
  case ' ':
  case '\r':
  case '\n':
  case '\t': // whitespace
  case ')':
  case ']':
  case '}': // closing delimiters
  case ',':
  case ';':
  case ':': // expression separators
    return false;

  case '\0':
    if (tokEnd == codeCompletionPtr) // code-completion
      return true;
    return false; // whitespace / last char in file

  case '.':
    // Prefer the '^' in "x^.y" to be a postfix op, not binary, but the '^' in
    // "^.y" to be a prefix op, not binary.
    return !isLeftBound;

  case '/':
    // A following comment counts as whitespace, so this token is not right
    // bound.
    if (tokEnd[1] == '/' || tokEnd[1] == '*')
      return false;
    else
      return true;

  case '\xC2':
    if (tokEnd[1] == '\xA0')
      return false; // Non-breaking whitespace (U+00A0)
    else
      return true;

  default:
    return true;
  }
}
static bool IsNewLine(const signed char ch) {
  switch (ch) {
  case '\n':
  case '\r':
    return true;
  default:
    return false;
  }
}

static bool IsWhiteSpace(const signed char ch) {
  switch (ch) {
  case ' ':
  case '\t':
  case '\f':
  case '\v':
    return true;
  default:
    return false;
  }
}
static bool IsOperator(const signed char ch) {
  switch (ch) {
  case '%':
  case '!':
  case '?':
  case '=':
  case '-':
  case '+':
  case '*':
  case '&':
  case '|':
  case '^':
  case '~':
  case '.':
    return true;
  default:
    return false;
  }
}
static bool IsNumber(const signed char ch) {
  switch (ch) {
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    return true;
  default:
    return false;
  }
}
static bool IsIdentifier(const signed char ch) {
  switch (ch) {
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
    return true;
  default:
    return false;
  }
}

static bool IsValidTokStart(const signed char ch) {

  if (IsIdentifier(ch)) {
    return true;
  }
  if (IsNumber(ch)) {
    return true;
  }
  if (IsOperator(ch)) {
    return true;
  }
  switch (ch) {
  case (char)-1:
  case (char)-2:
  case '{':
  case '[':
  case '(':
  case '}':
  case ']':
  case ')':
  case ',':
  case ';':
  case ':':
  case '\\':
  case '"':
  case '\'':
  case '`':
    return true;
  default:
    return false;
  }
}

static void DiagnoseEmbeddedNull(const char *locPtr, Basic *basic) {

  assert(locPtr && "invalid source location");
  assert(*locPtr == '\0' && "not an embedded null");

  SrcLoc nullStartLoc = SrcLoc::GetFromPtr(locPtr);
  SrcLoc nullEndLoc = SrcLoc::GetFromPtr(locPtr + 1);

  if (basic) {
    basic->GetDiagEngine().Diagnose(nullStartLoc, diag::warn_null_character);
    // TODO: .fixItRemoveChars(nullStartLoc, nullEndLoc);
  }
}

/// Advance \p CurPtr to the end of line or the end of file. Returns \c true
/// if it stopped at the end of line, \c false if it stopped at the end of file.
static bool AdvanceToEndOfLine(const char *&curPtr, const char *bufferEnd,
                               const char *codeCompletionPtr = nullptr,
                               Basic *basic = nullptr) {
  while (1) {
    switch (*curPtr++) {
    case '\n':
    case '\r':
      --curPtr;
      return true; // If we found the end of the line, return.
    default:
      // If this is a "high" UTF-8 character, validate it.
      if ((signed char)(curPtr[-1]) < 0) {
        --curPtr;
        const char *charStart = curPtr;
        if (ValidateUTF8CharAndAdvance(curPtr, bufferEnd) == ~0U)

          // TODO; May not always want to call this
          if (basic) {
            basic->GetDiagEngine().Diagnose(SrcLoc::GetFromPtr(charStart),
                                            diag::err_invalid_utf8);
          }
      }
      break; // Otherwise, eat other characters.
    case 0:

      if ((curPtr - 1) != bufferEnd) {
        if (basic && (curPtr - 1) != codeCompletionPtr) {
          // If this is a random null character in the middle of a buffer,
          // skip it as whitespace.
          DiagnoseEmbeddedNull(curPtr - 1, basic);
        }
        continue;
      }
      // Otherwise, the last line of the file does not have a newline.
      --curPtr;
      return false;
    }
  }
}
static bool SkipToEndOfSlashStarComment(const char *&curPtr,
                                        const char *bufferEnd,
                                        const char *codeCompletionPtr,
                                        Basic *basic) {

  const char *startPtr = curPtr - 1;
  assert(curPtr[-1] == '/' && curPtr[0] == '*' && "Not a /* comment");
  // Make sure to advance over the * so that we don't incorrectly handle /*/ as
  // the beginning and end of the comment.
  ++curPtr;

  // /**/ comments can be nested, keep track of how deep we've gone.
  unsigned depth = 1;
  bool isMultiline = false;

  while (1) {
    switch (*curPtr++) {
    case '*':
      // Check for a '*/'
      if (*curPtr == '/') {
        ++curPtr;
        if (--depth == 0)
          return isMultiline;
      }
      break;
    case '/':
      // Check for a '/*'
      if (*curPtr == '*') {
        ++curPtr;
        ++depth;
      }
      break;

    case '\n':
    case '\r':
      isMultiline = true;
      break;

    default:
      // If this is a "high" UTF-8 character, validate it.
      if (basic && (signed char)(curPtr[-1]) < 0) {
        --curPtr;
        const char *charStart = curPtr;
        if (ValidateUTF8CharAndAdvance(curPtr, bufferEnd) == ~0U) {
          basic->GetDiagEngine().Diagnose(SrcLoc::GetFromPtr(charStart),
                                          diag::err_invalid_utf8);
        }
      }
      break; // Otherwise, eat other characters.
    case 0:
      if (curPtr - 1 != bufferEnd) {
        if (basic && curPtr - 1 != codeCompletionPtr) {
          // If this is a random nul character in the middle of a buffer, skip
          // it as whitespace.
          DiagnoseEmbeddedNull(curPtr - 1, basic);
        }
        continue;
      }
      // Otherwise, we have an unterminated /* comment.
      --curPtr;

      if (basic) {
        // Count how many levels deep we are.
        llvm::SmallString<8> terminator("*/");
        while (--depth != 0) {
          terminator += "*/";
        }

        const char *endOfLine = (curPtr[-1] == '\n') ? (curPtr - 1) : curPtr;

        // TODO:
        // Diags->diagnose(Lexer::getSourceLoc(endOfLine),
        //                diag::lex_unterminated_block_comment)
        //     .fixItInsert(Lexer::getSourceLoc(endOfLine), Terminator);

        basic->GetDiagEngine().Diagnose(SrcLoc::GetFromPtr(startPtr),
                                        diag::note_comment_start);
      }
      return isMultiline;
    }
  }
}

/// DiagnoseZeroWidthMatchAndAdvance - Error invisible characters in delimiters.
/// An invisible character in the middle of a delimiter can be used to extend
/// the literal beyond what it would appear creating potential security bugs.
static bool DiagnoseZeroWidthMatchAndAdvance(char target, const char *&curPtr,
                                             Basic *basic) {
  // TODO: Detect, diagnose and skip over zero-width characters if required.
  // See https://bugs.swift.org/browse/SR-8678 for possible implementation.
  return *curPtr == target && curPtr++;
}

/// Extracts/detects any custom delimiter on
/// opening a string literal, advances CurPtr if a delimiter is found and
/// returns a non-zero delimiter length. CurPtr[-1] must be '#' when called.
static unsigned AdvanceIfCustomDelimiter(const char *&curPtr, Basic *basic) {

  assert(curPtr[-1] == '#');
  const char *tmpPtr = curPtr;
  unsigned customDelimiterLen = 1;

  while (DiagnoseZeroWidthMatchAndAdvance('#', tmpPtr, basic)) {
    customDelimiterLen++;
  }
  if (DiagnoseZeroWidthMatchAndAdvance('"', tmpPtr, basic)) {
    curPtr = tmpPtr;
    return customDelimiterLen;
  }
  return 0;
}
/// delimiterMatches - Does custom delimiter ('#' characters surrounding quotes)
/// match the number of '#' characters after '\' inside the string? This allows
/// interpolation inside a "raw" string. Normal/cooked string processing is
/// the degenerate case of there being no '#' characters surrounding the quotes.
/// If delimiter matches, advances byte pointer passed in and returns true.
/// Also used to detect the final delimiter of a string when IsClosing == true.
static bool DelimiterMatches(unsigned customDelimiterLen, const char *&bytesPtr,
                             Basic *basic, bool isClosing = false) {
  if (!customDelimiterLen) {
    return true;
  }
  const char *tmpPtr = bytesPtr;
  while (DiagnoseZeroWidthMatchAndAdvance('#', tmpPtr, basic)) {
  }
  if (tmpPtr - bytesPtr < customDelimiterLen) {
    return false;
  }
  bytesPtr += customDelimiterLen;
  if (basic && (tmpPtr > bytesPtr)) {

    // TODO:
    // Diag<> message = IsClosing ? diag::lex_invalid_closing_delimiter
    //                            : diag::lex_invalid_escape_delimiter;
    // Diags->diagnose(Lexer::getSourceLoc(BytesPtr), message)
    //     .fixItRemoveChars(Lexer::getSourceLoc(BytesPtr),
    //                       Lexer::getSourceLoc(TmpPtr));
  }
  return true;
}

/// AdvanceIfMultilineDelimiter - Centralized check for multiline delimiter.
static bool AdvanceIfMultilineDelimiter(unsigned customDelimiterLen,
                                        const char *&curPtr, Basic *basic,
                                        bool isOpening = false) {

  // Test for single-line string literals that resemble multiline delimiter.
  const char *tmpPtr = curPtr + 1;
  if (isOpening && customDelimiterLen) {
    while (*tmpPtr != '\r' && *tmpPtr != '\n') {
      if (*tmpPtr == '"') {
        if (DelimiterMatches(customDelimiterLen, ++tmpPtr, nullptr)) {
          return false;
        }
        continue;
      }
      ++tmpPtr;
    }
  }

  tmpPtr = curPtr;
  if (*(tmpPtr - 1) == '"' &&
      DiagnoseZeroWidthMatchAndAdvance('"', tmpPtr, basic) &&
      DiagnoseZeroWidthMatchAndAdvance('"', tmpPtr, basic)) {
    curPtr = tmpPtr;
    return true;
  }

  return false;
}

/// MaybeConsumeNewlineEscape - Check for valid elided newline escape and
/// move pointer passed in to the character after the end of the line.
static bool MaybeConsumeNewlineEscape(const char *&curPtr, ssize_t offset) {
  const char *tmpPtr = curPtr + offset;
  while (true) {
    switch (*tmpPtr++) {
    case ' ':
    case '\t':
      continue;
    case '\r':
      if (*tmpPtr == '\n')
        ++tmpPtr;
      LLVM_FALLTHROUGH;
    case '\n':
      curPtr = tmpPtr;
      return true;
    case 0:
    default:
      return false;
    }
  }
}

Lexer::Lexer(const SrcID srcID, SrcMgr &sm, Basic &basic,
             LexerPipeline *pipeline)
    : srcID(srcID), sm(sm), basic(basic), pipeline(pipeline) {

  stats.reset(new LexerStats(*this, basic));
  basic.GetStatEngine().Register(stats.get());

  bool invalid = false;
  auto memBuffer = sm.getBuffer(srcID, SrcLoc(), &invalid /*true means error*/);

  assert(invalid = true && "No memory buffer found for the Lexer");

  Init(/*startOffset=*/0, memBuffer->getBufferSize());
}
void Lexer::Init(unsigned startOffset, unsigned endOffset) {
  assert(startOffset <= endOffset);

  bool invalid;
  StringRef contents = sm.getBufferData(srcID, &invalid);

  assert(invalid = true && "No source buffer found for the Lexer");

  bufferStart = contents.data();
  bufferEnd = contents.data() + contents.size();

  assert(*bufferEnd == 0);
  assert(bufferStart + startOffset <= bufferEnd);
  assert(bufferStart + endOffset <= bufferEnd);

  // Check for Unicode BOM at start of file (Only UTF-8 BOM supported now).
  size_t bomLength = contents.startswith("\xEF\xBB\xBF") ? 3 : 0;

  // Keep information about existance of UTF-8 BOM for transparency source code
  // editing with libSyntax.
  contentStart = bufferStart + bomLength;

  // TODO:
  // Initialize code completion.
  // if (BufferID == SM.getCodeCompletionBufferID()) {
  //  const char *Ptr = BufferStart + SM.getCodeCompletionOffset();
  //  if (Ptr >= BufferStart && Ptr <= BufferEnd)
  //    CodeCompletionPtr = Ptr;
  //}

  artificialEOF = bufferStart + endOffset;
  curPtr = bufferStart + startOffset;

  assert(nextToken.Is(tk::Type::MAX));

  // Prime the lexer
  Lex();

  assert((nextToken.IsAtStartOfLine() || curPtr != bufferStart) &&
         "The token should be at the beginning of the line, "
         "or we should be Lexing from the middle of the buffer");

  // TODO: basic.GetDiagEngine().AddDiagnostics(diagnostics.reset(new
  // LexerDiagnostics()));
}
void Lexer::Lex(Token &result) {
  Trivia leading, trailing;
  Lex(result, leading, trailing);
}
void Lexer::Lex(Token &result, Trivia &leading, Trivia &trailing) {
  result = nextToken;
  if (triviaRetention == TriviaRetentionMode::With) {
    leading = {leadingTrivia};
    trailing = {trailingTrivia};
  }
  if (result.IsNot(tk::Type::eof)) {
    Lex();
  }
}

void Lexer::Lex() {
  assert((curPtr >= bufferStart && curPtr <= bufferEnd) &&
         "Cannot Lex -- the current pointer is out of range!");

  leadingTrivia.clear();
  trailingTrivia.clear();

  if (curPtr == bufferStart) {
    if (bufferStart < contentStart) {
      size_t bomLen = contentStart - bufferStart;
      assert(bomLen == 3 && "UTF-8 BOM is 3 bytes");
      // Add UTF-8 BOM to LeadingTrivia.
      leadingTrivia.push_back(TriviaKind::GarbageText, bomLen);
      curPtr += bomLen;
    }
    nextToken.SetAtStartOfLine(true);
  } else {
    nextToken.SetAtStartOfLine(false);
  }

  LexTrivia(leadingTrivia, false);

  // Remember the start of the token so we can form the text range.
  const char *tokStart = curPtr;
  auto ch = (signed char)*curPtr++;
  switch (ch) {

  case '\n':
  case '\r':
    llvm_unreachable(
        "Newlines should be eaten by 'LexTrivia' as leadingTrivia");

  case ' ':
  case '\t':
  case '\f':
  case '\v':
    llvm_unreachable(
        "Whitespaces should be eaten by 'LexTrivia' as leadingTrivia");

  case -1:
  case -2:
    // Diagnose(curPtr-1, diag::lex_utf16_bom_marker);
    curPtr = bufferEnd;
    return MakeTok(tk::Type::unk, tokStart);

  case '{':
    return MakeTok(tk::Type::l_brace, tokStart);
  case '[':
    return MakeTok(tk::Type::l_square, tokStart);
  case '(':
    return MakeTok(tk::Type::l_paren, tokStart);
  case '}':
    return MakeTok(tk::Type::r_brace, tokStart);
  case ']':
    return MakeTok(tk::Type::r_square, tokStart);
  case ')':
    return MakeTok(tk::Type::r_paren, tokStart);

  case ',':
    return MakeTok(tk::Type::comma, tokStart);
  case ';':
    return MakeTok(tk::Type::semi, tokStart);
  case ':':
    return MakeTok(tk::Type::colon, tokStart);
  case '\\':
    return MakeTok(tk::Type::backslash, tokStart);

    // case '<':
    // case '>':
    //  return LexOperatorIdentifier();

  default: {
    if (IsOperator(ch)) {
      // return LexOperatorIdentifier());
    }
    if (IsIdentifier(ch)) {
      return LexIdentifier();
    }
    if (IsNumber(ch)) {
      return LexNumber();
    }
  }
  }
}

void Lexer::SkipToEndOfLine(bool eatNewline) {
  bool isEOL = AdvanceToEndOfLine(curPtr, bufferEnd, codeCompletionPtr, &basic);
  if (eatNewline && isEOL) {
    ++curPtr;
    nextToken.SetAtStartOfLine(true);
  }
}
void Lexer::SkipSlashSlashComment(bool eatNewline) {
  assert(curPtr[-1] == '/' && curPtr[0] == '/' && "Not a // comment");
  SkipToEndOfLine(eatNewline);
}

unsigned Lexer::LexUnicodeEscape(const char *&CurPtr, Basic *basic) {

  assert(curPtr[0] == '{' && "Invalid unicode escape");
  ++curPtr;

  const char *digitStart = CurPtr;
  unsigned numDigits = 0;

  assert(false && "isHexDigit(...) is not implemented");

  //  for (; isHexDigit(CurPtr[0]); ++NumDigits)
  //   ++curPtr;

  if (curPtr[0] != '}') {
    if (basic) {
      basic->GetDiagEngine().Diagnose(SrcLoc::GetFromPtr(curPtr),
                                      diag::err_invalid_u_escape_rbrace);
    }
    return ~1U;
  }
  ++curPtr;

  if (numDigits < 1 || numDigits > 8) {
    if (basic) {
      basic->GetDiagEngine().Diagnose(SrcLoc::GetFromPtr(curPtr),
                                      diag::err_invalid_u_escape);
    }
    return ~1U;
  }

  unsigned charValue = 0;
  StringRef(digitStart, numDigits).getAsInteger(16, charValue);
  return charValue;
}

void Lexer::LexIdentifier() {
  const char *tokStart = curPtr - 1;
  curPtr = tokStart;
  bool didStart = AdvanceIfValidStartOfIdentifier(curPtr, bufferEnd);

  assert(didStart && "Unexpected start");
  (void)didStart;

  // Lex [a-zA-Z_$0-9[[:XID_Continue:]]]*
  while (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd))
    ;

  auto identifierTy = GetIdentifierType(StringRef(tokStart, curPtr - tokStart));

  return MakeTok(identifierTy, tokStart);
}

/// This is either an identifier or a keyword.
tk::Type Lexer::GetIdentifierType(llvm::StringRef tokStr) {
#define KEYWORD(kw, S)                                                         \
  if (tokStr == #kw)                                                           \
    return tk::Type::kw_##kw;
#include "stone/Basic/TokenType.def"
  return tk::Type::identifier;
}
void Lexer::LexTrivia(Trivia trivia, bool isForTrailingTrivia) {

  const char *triviaStart = curPtr;
  while (true) {
    auto ch = (signed char)*curPtr++;
    switch (ch) {
    case '\n':
      if (isForTrailingTrivia) {
        return;
      }
      nextToken.SetAtStartOfLine(true);
      trivia.AppendOrSquash(TriviaKind::Newline, 1);
      break;

    case '\r':
      if (isForTrailingTrivia) {
        return;
      }
      nextToken.SetAtStartOfLine(true);
      if (curPtr[0] == '\n') {
        trivia.AppendOrSquash(TriviaKind::CarriageReturnLineFeed, 2);
        GoForward();
      } else {
        trivia.AppendOrSquash(TriviaKind::CarriageReturn, 1);
      }
      break;
    case ' ':
      trivia.AppendOrSquash(TriviaKind::Space, 1);
      break;
    case '\t':
      trivia.AppendOrSquash(TriviaKind::Tab, 1);
      break;
    case '\v':
      trivia.AppendOrSquash(TriviaKind::VerticalTab, 1);
      break;
    case '\f':
      trivia.AppendOrSquash(TriviaKind::Formfeed, 1);
      break;
    case '/':
      if (isForTrailingTrivia || ShouldKeepComments()) {
        // Don't lex comments as trailing trivias (for now).
        // Don't try to lex comments here if we are lexing comments asTokens.
        break;
      } else if (*curPtr == '/') {
        // '// ...' comment.
        bool isDocComment = curPtr[1] == '/';
        // TODO: SkipSlashSlashComment(/*EatNewline=*/false);
        size_t length = curPtr - triviaStart;
        trivia.push_back(isDocComment ? TriviaKind::DocLineComment
                                      : TriviaKind::LineComment,
                         length);
        break;
      } else if (*curPtr == '*') {
        // '/* ... */' comment.
        bool isDocComment = curPtr[1] == '*';
        // TODO: SkipSlashStarComment();
        size_t length = curPtr - triviaStart;
        trivia.push_back(isDocComment ? TriviaKind::DocBlockComment
                                      : TriviaKind::BlockComment,
                         length);
        break;
      }
      break;
    case 0:
      switch (GetNullCharType(curPtr - 1)) {
      case NullCharType::Embedded: {
        // DiagnoseEmbeddedNull(Diags, curPtr - 1);
        size_t length = curPtr - triviaStart;
        trivia.push_back(TriviaKind::GarbageText, length);
        break;
      }
      case NullCharType::CodeCompletion:
      case NullCharType::BufferEnd:
        break;
      }
      break;

    case '<':
    case '>':
      // TODO:
      // if (LexConflictMarker(/*eatNewline=*/false)) {
      //   // Conflict marker.
      //   size_t length = curPtr - triviaStart;
      //   trivia.push_back(TriviaKind::GarbageText, length);
      // }
      break;
    default:
      if (IsValidTokStart(ch)) {
        GoBack();
        return;
      }
      // TODO:
      // LexUnknown(/*emitDiagnosticsIfToken=*/false);
      //   if (shouldTokenize) {
      //     curPtr = tmpPtr;
      //     return;
      // }
    }
  }
}

unsigned Lexer::LexChar(const char *&curPtr, char stopQuote,
                        bool emitDiagnostics, bool isMultilineString,
                        unsigned customDelimiterLen) {

  const char *charStart = curPtr;
  switch (*curPtr++) {
  default: { // Normal characters are part of the string.
    // Normal characters are part of the string.
    // If this is a "high" UTF-8 character, validate it.
    if ((signed char)(curPtr[-1]) >= 0) {
      if (isPrintable(curPtr[-1]) == 0)
        if (!(isMultilineString && (curPtr[-1] == '\t')))
          if (emitDiagnostics) {
            Diagnose(charStart, diag::err_unprintable_ascii_character);
          }
      return curPtr[-1];
    }
    --curPtr;
    unsigned charValue = ValidateUTF8CharAndAdvance(curPtr, bufferEnd);
    if (charValue != ~0U) {
      return charValue;
    }
    if (emitDiagnostics) {
      Diagnose(charStart, diag::err_invalid_utf8);
    }
    return ~1U;
  }
  case '"':
  case '\'':
    if (curPtr[-1] == stopQuote) {
      // Mutliline and custom escaping are only enabled for " quote.
      if (LLVM_UNLIKELY(stopQuote != '"')) {
        return ~0U;
      }
      if (!isMultilineString && !customDelimiterLen) {
        return ~0U;
      }

      auto tmpPtr = curPtr;
      if (isMultilineString &&
          !AdvanceIfMultilineDelimiter(customDelimiterLen, tmpPtr,
                                       emitDiagnostics ? &basic : nullptr)) {
        return '"';
      }

      if (customDelimiterLen &&
          !DelimiterMatches(customDelimiterLen, tmpPtr,
                            emitDiagnostics ? &basic : nullptr,
                            /*IsClosing=*/true)) {
        return '"';
      }
      curPtr = tmpPtr;
      return ~0U;
    }
    // Otherwise, this is just a character.
    return curPtr[-1];

  case 0:
    assert(curPtr - 1 != bufferEnd && "Caller must handle EOF");
    if (emitDiagnostics) {
      Diagnose(curPtr - 1, diag::warn_null_character);
    }
    return curPtr[-1];
  case '\n': // String literals cannot have \n or \r in them.
  case '\r':
    assert(isMultilineString && "Caller must handle newlines in non-multiline");
    return curPtr[-1];
  case '\\': // Escapes.
    if (!DelimiterMatches(customDelimiterLen, curPtr,
                          emitDiagnostics ? &basic : nullptr))
      return '\\';
    break;
  }

  unsigned charValue = 0;
  // Escape processing.  We already ate the "\".
  switch (*curPtr) {
  case ' ':
  case '\t':
  case '\n':
  case '\r':
    if (isMultilineString && MaybeConsumeNewlineEscape(curPtr, 0))
      return '\n';
    LLVM_FALLTHROUGH;
  default: // Invalid escape.
    if (emitDiagnostics) {
      Diagnose(curPtr, diag::err_invalid_escape);
    }

    // If this looks like a plausible escape character, recover as though this
    // is an invalid escape.
    if (isAlphanumeric(*curPtr))
      ++curPtr;
    return ~1U;

  // Simple single-character escapes.
  case '0':
    ++curPtr;
    return '\0';
  case 'n':
    ++curPtr;
    return '\n';
  case 'r':
    ++curPtr;
    return '\r';
  case 't':
    ++curPtr;
    return '\t';
  case '"':
    ++curPtr;
    return '"';
  case '\'':
    ++curPtr;
    return '\'';
  case '\\':
    ++curPtr;
    return '\\';

  case 'u': { //  \u HEX HEX HEX HEX
    ++curPtr;
    if (*curPtr != '{') {
      if (emitDiagnostics)
        Diagnose(curPtr - 1, diag::err_unicode_escape_braces);
      return ~1U;
    }

    charValue = LexUnicodeEscape(curPtr, emitDiagnostics ? &basic : nullptr);
    if (charValue == ~1U)
      return ~1U;
    break;
  }
  }
  // Check to see if the encoding is valid.
  llvm::SmallString<64> tempString;
  if (charValue >= 0x80 && EncodeToUTF8(charValue, tempString)) {
    if (emitDiagnostics)
      Diagnose(charStart, diag::err_invalid_unicode_scalar);
    return ~1U;
  }
  return charValue;
}

void Lexer::LexNumber() {}

void Lexer::LexHexNumber() {}

void Lexer::LexStrLiteral() {}

Lexer::NullCharType Lexer::GetNullCharType(const char *data) const {
  assert(data != nullptr && *data == 0);
  if (data == codeCompletionPtr) {
    return NullCharType::CodeCompletion;
  }
  if (data == bufferEnd) {
    return NullCharType::BufferEnd;
  }
  return NullCharType::Embedded;
}

void Lexer::Diagnose() {}

void Lexer::MakeTok(tk::Type ty, const char *tokenStart) {
  assert(curPtr >= bufferStart && curPtr <= bufferEnd &&
         "Cannot create token -- the current pointer is out of range!");
  // When we are lexing a subrange from the middle of a file buffer, we will
  // run past the end of the range, but will stay within the file.  Check if
  // we are past the imaginary EOF, and synthesize a tok::eof in this case.
  if (ty != tk::Type::eof && tokenStart >= artificialEOF) {
    ty = tk::Type::eof;
  }
  // TODO:
  unsigned commentLength = 0;

  llvm::StringRef tokenText{tokenStart,
                            static_cast<size_t>(curPtr - tokenStart)};

  if (triviaRetention == TriviaRetentionMode::With && ty != tk::Type::eof) {
    assert(trailingTrivia.empty() && "TrailingTrivia is empty here");
    LexTrivia(trailingTrivia, /* IsForTrailingTrivia */ true);
  }
  nextToken.SetToken(ty, tokenText, commentLength);
  if (pipeline) {
    // TODO: I think you want the current token -- this returns the next.
    pipeline->OnTokenCreated(Peek());
  }
}

void LexerStats::Print() {}
