#include "stone/Compile/Lexer.h"

#include "stone/Compile/SyntaxListener.h"
#include "stone/Core/Char.h"
#include "stone/Core/SrcMgr.h"
#include "stone/Core/SyntaxDiagnostic.h"

using namespace stone;
using namespace stone::syn;

static bool EncodeToUTF8(unsigned charValue, SmallVectorImpl<char> &Result) {
  // Number of bits in the value, ignoring leading zeros.
  unsigned NumBits = 32 - llvm::countLeadingZeros(charValue);

  // Handle the leading byte, based on the number of bits in the value.
  unsigned NumTrailingBytes;
  if (NumBits <= 5 + 6) {
    // Encoding is 0x110aaaaa 10bbbbbb
    Result.push_back(char(0xC0 | (charValue >> 6)));
    NumTrailingBytes = 1;
  } else if (NumBits <= 4 + 6 + 6) {
    // Encoding is 0x1110aaaa 10bbbbbb 10cccccc
    Result.push_back(char(0xE0 | (charValue >> (6 + 6))));
    NumTrailingBytes = 2;

    // UTF-16 surrogate pair values are not valid code points.
    if (charValue >= 0xD800 && charValue <= 0xDFFF)
      return true;
    // U+FDD0...U+FDEF are also reserved
    if (charValue >= 0xFDD0 && charValue <= 0xFDEF)
      return true;
  } else if (NumBits <= 3 + 6 + 6 + 6) {
    // Encoding is 0x11110aaa 10bbbbbb 10cccccc 10dddddd
    Result.push_back(char(0xF0 | (charValue >> (6 + 6 + 6))));
    NumTrailingBytes = 3;
    // Reject over-large code points.  These cannot be encoded as UTF-16
    // surrogate pairs, so UTF-32 doesn't allow them.
    if (charValue > 0x10FFFF)
      return true;
  } else {
    return true; // UTF8 can encode these, but they aren't valid code points.
  }

  // Emit all of the trailing bytes.
  while (NumTrailingBytes--)
    Result.push_back(
        char(0x80 | (0x3F & (charValue >> (NumTrailingBytes * 6)))));
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
  unsigned charValue = (unsigned char)(CurByte << EncodedBytes) >> EncodedBytes;

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
    charValue <<= 6;
    charValue |= CurByte & 0x3F;
    ++Ptr;
  }

  // UTF-16 surrogate pair values are not valid code points.
  if (charValue >= 0xD800 && charValue <= 0xDFFF)
    return ~0U;

  // If we got here, we read the appropriate number of accumulated bytes.
  // Verify that the encoding was actually minimal.
  // Number of bits in the value, ignoring leading zeros.
  unsigned NumBits = 32 - llvm::countLeadingZeros(charValue);

  if (NumBits <= 5 + 6)
    return EncodedBytes == 2 ? charValue : ~0U;
  if (NumBits <= 4 + 6 + 6)
    return EncodedBytes == 3 ? charValue : ~0U;
  return EncodedBytes == 4 ? charValue : ~0U;
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

static bool IsOperatorStartCodePoint(uint32_t C) {
  // ASCII operator chars.
  static const char OpChars[] = "/=-+*%<>!&|^~.?";
  if (C < 0x80)
    return memchr(OpChars, C, sizeof(OpChars) - 1) != 0;

  // Unicode math, symbol, arrow, dingbat, and line/box drawing chars.
  return (C >= 0x00A1 && C <= 0x00A7) || C == 0x00A9 || C == 0x00AB ||
         C == 0x00AC || C == 0x00AE || C == 0x00B0 || C == 0x00B1 ||
         C == 0x00B6 || C == 0x00BB || C == 0x00BF || C == 0x00D7 ||
         C == 0x00F7 || C == 0x2016 || C == 0x2017 ||
         (C >= 0x2020 && C <= 0x2027) || (C >= 0x2030 && C <= 0x203E) ||
         (C >= 0x2041 && C <= 0x2053) || (C >= 0x2055 && C <= 0x205E) ||
         (C >= 0x2190 && C <= 0x23FF) || (C >= 0x2500 && C <= 0x2775) ||
         (C >= 0x2794 && C <= 0x2BFF) || (C >= 0x2E00 && C <= 0x2E7F) ||
         (C >= 0x3001 && C <= 0x3003) || (C >= 0x3008 && C <= 0x3030);
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

static bool AdvanceIfValidStartOfOperator(char const *&ptr, char const *end) {
  return AdvanceIf(ptr, end, IsOperatorStartCodePoint);
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

static bool IsValidtokStart(const signed char ch) {
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

static void DiagnoseEmbeddedNull(const char *locPtr, DiagnosticEngine *de) {
  assert(locPtr && "invalid source location");
  assert(*locPtr == '\0' && "not an embedded null");

  SrcLoc nullStartLoc = SrcLoc::GetFromPtr(locPtr);
  SrcLoc nullEndLoc = SrcLoc::GetFromPtr(locPtr + 1);

  if (de) {
    de->PrintD(nullStartLoc, diag::warn_null_character);
    // TODO: .fixItRemoveChars(nullStartLoc, nullEndLoc);
  }
}

/// Advance \p curPtr to the end of line or the end of file. Returns \c true
/// if it stopped at the end of line, \c false if it stopped at the end of file.
static bool AdvanceToEndOfLine(const char *&curPtr, const char *bufferEnd,
                               const char *codeCompletionPtr = nullptr,
                               DiagnosticEngine *de = nullptr) {
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
          if (de) {
            de->PrintD(SrcLoc::GetFromPtr(charStart),
                                        diag::err_invalid_utf8);
          }
      }
      break; // Otherwise, eat other characters.
    case 0:

      if ((curPtr - 1) != bufferEnd) {
        if (de && (curPtr - 1) != codeCompletionPtr) {
          // If this is a random null character in the middle of a buffer,
          // skip it as whitespace.
          DiagnoseEmbeddedNull(curPtr - 1, de);
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
                                        DiagnosticEngine *de) {
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
      if (de && (signed char)(curPtr[-1]) < 0) {
        --curPtr;
        const char *charStart = curPtr;
        if (ValidateUTF8CharAndAdvance(curPtr, bufferEnd) == ~0U) {
          de->PrintD(SrcLoc::GetFromPtr(charStart),
                                      diag::err_invalid_utf8);
        }
      }
      break; // Otherwise, eat other characters.
    case 0:
      if (curPtr - 1 != bufferEnd) {
        if (de && curPtr - 1 != codeCompletionPtr) {
          // If this is a random nul character in the middle of a buffer, skip
          // it as whitespace.
          DiagnoseEmbeddedNull(curPtr - 1, de);
        }
        continue;
      }
      // Otherwise, we have an unterminated /* comment.
      --curPtr;

      if (de) {
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

        de->PrintD(SrcLoc::GetFromPtr(startPtr),
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
                                             DiagnosticEngine *de) {
  // TODO: Detect, diagnose and skip over zero-width characters if required.
  // See https://bugs.swift.org/browse/SR-8678 for possible implementation.
  return *curPtr == target && curPtr++;
}

/// Extracts/detects any custom delimiter on
/// opening a string literal, advances curPtr if a delimiter is found and
/// returns a non-zero delimiter length. curPtr[-1] must be '#' when called.
static unsigned AdvanceIfCustomDelimiter(const char *&curPtr,
                                         DiagnosticEngine *de) {
  assert(curPtr[-1] == '#');
  const char *tmpPtr = curPtr;
  unsigned customDelimiterLen = 1;

  while (DiagnoseZeroWidthMatchAndAdvance('#', tmpPtr, de)) {
    customDelimiterLen++;
  }
  if (DiagnoseZeroWidthMatchAndAdvance('"', tmpPtr, de)) {
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
                             DiagnosticEngine *de, bool isClosing = false) {
  if (!customDelimiterLen) {
    return true;
  }
  const char *tmpPtr = bytesPtr;
  while (DiagnoseZeroWidthMatchAndAdvance('#', tmpPtr, de)) {
  }
  if (tmpPtr - bytesPtr < customDelimiterLen) {
    return false;
  }
  bytesPtr += customDelimiterLen;
  if (de && (tmpPtr > bytesPtr)) {

    stone::Panic("lex_invalid_closing_delimiter");
    // TODO:
    // Diag<> message = IsClosing ? diag::lex_invalid_closing_delimiter
    //                            : diag::lex_invalid_escape_delimiter;
    // Diags->diagnose(Lexer::getSourceLoc(BytesPtr), message)
    //     .fixItRemoveChars(Lexer::getSourceLoc(BytesPtr),
    //                       Lexer::getSourceLoc(tmpPtr));
  }
  return true;
}

/// AdvanceIfMultilineDelimiter - Centralized check for multiline delimiter.
static bool AdvanceIfMultilineDelimiter(unsigned customDelimiterLen,
                                        const char *&curPtr,
                                        DiagnosticEngine *de,
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
      DiagnoseZeroWidthMatchAndAdvance('"', tmpPtr, de) &&
      DiagnoseZeroWidthMatchAndAdvance('"', tmpPtr, de)) {
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

// TODO: Remove sm -- it is already in the de
Lexer::Lexer(PrincipalCtor &, const unsigned srcID, const SrcMgr &sm,
             DiagnosticEngine *de, StatisticEngine *se,
             SyntaxListener *pipeline)
    : srcID(srcID), sm(sm), de(de), se(se) {}

Lexer::Lexer(const unsigned srcID, const SrcMgr &sm, DiagnosticEngine *de,
             StatisticEngine *se, SyntaxListener *pipeline)
    : srcID(srcID), sm(sm), de(de), se(se), pipeline(pipeline) {

  if (se) {
    stats = std::make_unique<LexerStats>(*this);
    se->Register(stats.get());
  }

  unsigned endOffset = sm.getRangeForBuffer(srcID).getByteLength();
  Initialize(/*startOffset=*/0, endOffset);
}
void Lexer::Initialize(unsigned startOffset, unsigned endOffset) {
  assert(startOffset <= endOffset);

  StringRef contents = sm.extractText(sm.getRangeForBuffer(srcID));

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

  // Initialize code completion.
  if (srcID == sm.getCodeCompletionBufferID()) {
    const char *Ptr = bufferStart + sm.getCodeCompletionOffset();
    if (Ptr >= bufferStart && Ptr <= bufferEnd)
      codeCompletionPtr = Ptr;
  }

  artificialEOF = bufferStart + endOffset;
  curPtr = bufferStart + startOffset;

  assert(nextToken.Is(tok::MAX));

  // Prime the lexer
  Lex();
  assert((nextToken.IsAtStartOfLine() || curPtr != bufferStart) &&
         "The token should be at the beginning of the line, "
         "or we should be lexing from the middle of the buffer");
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
  if (result.IsNot(tok::eof)) {
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

  assert(!IsNewLine(ch) &&
         "Newlines should be eaten by 'LexTrivia' as leadingTrivia");
  assert(!IsWhiteSpace(ch) &&
         "Whitespaces should be eaten by 'LexTrivia' as leadingTrivia");

  switch (ch) {
  case -1:
  case -2:
    // PrintD(curPtr-1, diag::lex_utf16_bom_marker);
    curPtr = bufferEnd;
    return MakeTok(tok::alien, tokStart);
  case 0:
    switch (GetNullCharKind(curPtr - 1)) {
    case NullCharKind::CodeCompletion:
      while (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd))
        ;
      return MakeTok(tok::code_complete, tokStart);

    case NullCharKind::BufferEnd:
      // This is the real end of the buffer.
      // Put curPtr back into buffer bounds.
      --curPtr;
      // Return EOF.
      return MakeTok(tok::eof, tokStart);

    case NullCharKind::Embedded:
      stone::Panic(
          "Embedded nul should be eaten by lexTrivia as LeadingTrivia");
    }
  case '{':
    return MakeTok(tok::l_brace, tokStart);
  case '[':
    return MakeTok(tok::l_square, tokStart);
  case '(':
    return MakeTok(tok::l_paren, tokStart);
  case '}':
    return MakeTok(tok::r_brace, tokStart);
  case ']':
    return MakeTok(tok::r_square, tokStart);
  case ')':
    return MakeTok(tok::r_paren, tokStart);
  case ',':
    return MakeTok(tok::comma, tokStart);
  case ';':
    return MakeTok(tok::semi, tokStart);
  case ':':
    return MakeTok(tok::colon, tokStart);
  case '\\':
    return MakeTok(tok::backslash, tokStart);
  case '<':
  case '>':
    // return LexOperatorIdentifier();
    return;
  case '"':
  case '\'':
    return LexStringLiteral();

  case '`':
    return LexEscapedIdentifier();
  default:
    break;
  }

  if (IsNumber(ch)) {
    return LexNumber();
  }

  char const *tmpPtr = curPtr - 1;
  if (IsIdentifier(ch) && AdvanceIfValidStartOfIdentifier(tmpPtr, bufferEnd)) {
    return LexIdentifier();
  }

  if (IsOperator(ch) && AdvanceIfValidStartOfOperator(tmpPtr, bufferEnd)) {
    // return LexOperatorIdentifier();
    return;
  }
}

void Lexer::SkipToEndOfLine(bool eatNewline) {
  bool isEOL = AdvanceToEndOfLine(curPtr, bufferEnd, codeCompletionPtr, de);
  if (eatNewline && isEOL) {
    ++curPtr;
    nextToken.SetAtStartOfLine(true);
  }
}
void Lexer::SkipSlashSlashComment(bool eatNewline) {
  assert(curPtr[-1] == '/' && curPtr[0] == '/' && "Not a // comment");
  SkipToEndOfLine(eatNewline);
}

unsigned Lexer::LexUnicodeEscape(const char *&curPtr, DiagnosticEngine *de) {
  assert(curPtr[0] == '{' && "Invalid unicode escape");
  ++curPtr;

  const char *digitStart = curPtr;
  unsigned numDigits = 0;

  assert(false && "isHexDigit(...) is not implemented");

  //  for (; isHexDigit(curPtr[0]); ++NumDigits)
  //   ++curPtr;

  if (curPtr[0] != '}') {
    if (de) {
      de->PrintD(SrcLoc::GetFromPtr(curPtr), diag::err_invalid_u_escape_rbrace);
    }
    return ~1U;
  }
  ++curPtr;

  if (numDigits < 1 || numDigits > 8) {
    if (de) {
      de->PrintD(SrcLoc::GetFromPtr(curPtr),
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

  assert(didStart && "Invalid start of identifier");
  (void)didStart;

  // Lex [a-zA-Z_$0-9[[:XID_Continue:]]]*
  while (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd))
    ;

  auto identifierTy = GetIdentifierType(StringRef(tokStart, curPtr - tokStart));

  return MakeTok(identifierTy, tokStart);
}

/// This is either an identifier or a keyword.
tok Lexer::GetIdentifierType(llvm::StringRef tokStr) {
#define KEYWORD(kw, S)                                                         \
  if (tokStr == #kw)                                                           \
    return tok::kw_##kw;
#include "stone/Core/TokenKind.def"
  return tok::identifier;
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
      switch (GetNullCharKind(curPtr - 1)) {
      case NullCharKind::Embedded: {
        // DiagnoseEmbeddedNull(Diags, curPtr - 1);
        size_t length = curPtr - triviaStart;
        trivia.push_back(TriviaKind::GarbageText, length);
        break;
      }
      case NullCharKind::CodeCompletion:
      case NullCharKind::BufferEnd:
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
      if (IsValidtokStart(ch)) {
        GoBack();
        return;
      }
      // TODO:
      // LexAlien(/*emitDiagnosticsIfToken=*/false);
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
            PrintD(charStart, diag::err_unprintable_ascii_character);
          }
      return curPtr[-1];
    }
    --curPtr;
    unsigned charValue = ValidateUTF8CharAndAdvance(curPtr, bufferEnd);
    if (charValue != ~0U) {
      return charValue;
    }
    if (emitDiagnostics) {
      PrintD(charStart, diag::err_invalid_utf8);
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
                                       emitDiagnostics ? de : nullptr)) {
        return '"';
      }

      if (customDelimiterLen &&
          !DelimiterMatches(customDelimiterLen, tmpPtr,
                            emitDiagnostics ? de : nullptr,
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
      PrintD(curPtr - 1, diag::warn_null_character);
    }
    return curPtr[-1];
  case '\n': // String literals cannot have \n or \r in them.
  case '\r':
    assert(isMultilineString && "Caller must handle newlines in non-multiline");
    return curPtr[-1];
  case '\\': // Escapes.
    if (!DelimiterMatches(customDelimiterLen, curPtr,
                          emitDiagnostics ? de : nullptr))
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
      PrintD(curPtr, diag::err_invalid_escape);
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
        PrintD(curPtr - 1, diag::err_unicode_escape_braces);
      return ~1U;
    }

    charValue = LexUnicodeEscape(curPtr, emitDiagnostics ? de : nullptr);
    if (charValue == ~1U)
      return ~1U;
    break;
  }
  }
  // Check to see if the encoding is valid.
  llvm::SmallString<64> tempString;
  if (charValue >= 0x80 && EncodeToUTF8(charValue, tempString)) {
    if (emitDiagnostics)
      PrintD(charStart, diag::err_invalid_unicode_scalar);
    return ~1U;
  }
  return charValue;
}

enum class ExpectedDigitKind : unsigned { Binary, Octal, Decimal, Hex };
void Lexer::LexNumber() {
  const char *tokStart = curPtr - 1;
  assert((isDigit(*tokStart) || *tokStart == '.') && "Unexpected start");

  auto ExpectedDigit = [&]() {
    while (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd))
      ;
    return MakeTok(tok::alien, tokStart);
  };

  auto ExpectedIntDigit = [&](const char *loc, ExpectedDigitKind kind) {
    // diagnose(loc, diag::lex_invalid_digit_in_int_literal, StringRef(loc, 1),
    //          (unsigned)kind);
    stone::Panic("lex_invalid_digit_in_int_literal");
    return ExpectedDigit();
  };

  if (*tokStart == '0' && *curPtr == 'x') {
    return LexHexNumber();
  }

  if (*tokStart == '0' && *curPtr == 'o') {
    // 0o[0-7][0-7_]*
    ++curPtr;

    if (*curPtr < '0' || *curPtr > '7') {
      return ExpectedIntDigit(curPtr, ExpectedDigitKind::Octal);
    }

    while ((*curPtr >= '0' && *curPtr <= '7') || *curPtr == '_') {
      ++curPtr;
    }

    auto tmpPtr = curPtr;
    if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
      return ExpectedIntDigit(tmpPtr, ExpectedDigitKind::Octal);
    }

    return MakeTok(tok::integer_literal, tokStart);
  }

  if (*tokStart == '0' && *curPtr == 'b') {
    // 0b[01][01_]*
    ++curPtr;
    if (*curPtr != '0' && *curPtr != '1') {
      return ExpectedIntDigit(curPtr, ExpectedDigitKind::Binary);
    }

    while (*curPtr == '0' || *curPtr == '1' || *curPtr == '_') {
      ++curPtr;
    }

    auto tmp = curPtr;
    if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
      return ExpectedIntDigit(tmp, ExpectedDigitKind::Binary);
    }

    return MakeTok(tok::integer_literal, tokStart);
  }

  // Handle a leading [0-9]+, lexing an integer or falling through if we have a
  // floating point value.
  while (isDigit(*curPtr) || *curPtr == '_') {
    ++curPtr;
  }

  // Lex things like 4.x as '4' followed by a tok::period.
  if (*curPtr == '.') {
    // nextToken is the soon to be previous token
    // Therefore: x.0.1 is sub-tuple access, not x.float_literal
    if (!isDigit(curPtr[1]) || nextToken.Is(tok::period)) {
      return MakeTok(tok::integer_literal, tokStart);
    }
  } else {
    // Floating literals must have '.', 'e', or 'E' after digits.  If it is
    // something else, then this is the end of the token.
    if (*curPtr != 'e' && *curPtr != 'E') {
      auto tmpPtr = curPtr;
      if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
        return ExpectedIntDigit(tmpPtr, ExpectedDigitKind::Decimal);
      }

      return MakeTok(tok::integer_literal, tokStart);
    }
  }

  // Lex decimal point.
  if (*curPtr == '.') {
    ++curPtr;

    // Lex any digits after the decimal point.
    while (isDigit(*curPtr) || *curPtr == '_') {
      ++curPtr;
    }
  }

  // Lex exponent.
  if (*curPtr == 'e' || *curPtr == 'E') {
    ++curPtr; // Eat the 'e'

    if (*curPtr == '+' || *curPtr == '-') {
      ++curPtr; // Eat the sign.
    }

    if (!isDigit(*curPtr)) {
      // There are 3 cases to diagnose if the exponent starts with a
      // non-digit:
      // identifier (invalid character), underscore (invalid first
      // character),
      // non-identifier (empty exponent)

      auto tmpPtr = curPtr;
      if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
        // stone::Panic("diagnose(tmpPtr,
        // diag::lex_invalid_digit_in_fp_exponent,
        //              StringRef(tmpPtr, 1),
        //              *tmpPtr == '_');
        // ");
      } else {
        // stone::Panic(
        //     "diagnose(curPtr, diag::lex_expected_digit_in_fp_exponent);");
      }

      return ExpectedDigit();
    }

    while (isDigit(*curPtr) || *curPtr == '_') {
      ++curPtr;
    }

    auto tmpPtr = curPtr;
    if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
      // stone::Panic("diagnose(tmpPtr,
      //              diag::lex_invalid_digit_in_fp_exponent,
      //              StringRef(tmp, 1), false);
      // ");

      return ExpectedDigit();
    }
  }

  return MakeTok(tok::floating_literal, tokStart);
}

void Lexer::LexHexNumber() {
  // We assume we're starting from the 'x' in a '0x...' floating-point
  // literal.
  assert(*curPtr == 'x' && "not a hex literal");
  const char *tokStart = curPtr - 1;
  assert(*tokStart == '0' && "not a hex literal");

  auto ExpectedDigit = [&]() {
    while (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd))
      ;
    return MakeTok(tok::alien, tokStart);
  };

  auto ExpectedHexDigit = [&](const char *loc) {
    PrintD(loc, diag::lex_invalid_character);

    // stone::Panic("diagnose(loc, diag::lex_invalid_digit_in_int_literal,
    // StringRef(loc, 1),
    //          (unsigned)ExpectedDigitKind::Hex);");

    return ExpectedDigit();
  };

  // 0x[0-9a-fA-F][0-9a-fA-F_]*
  ++curPtr;
  if (!isHexDigit(*curPtr)) {
    return ExpectedHexDigit(curPtr);
  }

  while (isHexDigit(*curPtr) || *curPtr == '_') {
    ++curPtr;
  }

  if (*curPtr != '.' && *curPtr != 'p' && *curPtr != 'P') {
    auto tmpPtr = curPtr;
    if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
      return ExpectedHexDigit(tmpPtr);
    } else {
      return MakeTok(tok::integer_literal, tokStart);
    }
  }

  const char *ptrOnDot = nullptr;

  // (\.[0-9A-Fa-f][0-9A-Fa-f_]*)?
  if (*curPtr == '.') {
    ptrOnDot = curPtr;
    ++curPtr;

    // If the character after the '.' is not a digit, assume we have an int
    // literal followed by a dot expression.
    if (!isHexDigit(*curPtr)) {
      --curPtr;
      return MakeTok(tok::integer_literal, tokStart);
    }

    while (isHexDigit(*curPtr) || *curPtr == '_') {
      ++curPtr;
    }

    if (*curPtr != 'p' && *curPtr != 'P') {
      if (!isDigit(ptrOnDot[1])) {
        // e.g: 0xff.description
        curPtr = ptrOnDot;
        return MakeTok(tok::integer_literal, tokStart);
      }
      // diagnose(curPtr,
      // diag::lex_expected_binary_exponent_in_hex_float_literal);

      return MakeTok(tok::alien, tokStart);
    }
  }

  // // [pP][+-]?[0-9][0-9_]*
  assert(*curPtr == 'p' || *curPtr == 'P' && "not at a hex float exponent?!");
  ++curPtr;

  bool signedExponent = false;
  if (*curPtr == '+' || *curPtr == '-') {
    ++curPtr; // Eat the sign.
    signedExponent = true;
  }

  if (!isDigit(*curPtr)) {
    if (ptrOnDot && !isDigit(ptrOnDot[1]) && !signedExponent) {
      // e.g: 0xff.fpValue, 0xff.fp
      curPtr = ptrOnDot;
      return MakeTok(tok::integer_literal, tokStart);
    }
    // Note: 0xff.fp+otherExpr can be valid expression. But we don't accept it.

    // There are 3 cases to diagnose if the exponent starts with a non-digit:
    // identifier (invalid character), underscore (invalid first character),
    // non-identifier (empty exponent)
    auto tmpPtr = curPtr;
    if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
      stone::Panic("lex_invalid_digit_in_fp_exponent");
      // diagnose(tmpPtr, diag::lex_invalid_digit_in_fp_exponent, StringRef(tmp,
      // 1),
      //          *tmpPtr == '_');
    } else {
      stone::Panic("lex_expected_digit_in_fp_exponent");
      // diagnose(curPtr, diag::lex_expected_digit_in_fp_exponent);
    }

    return ExpectedDigit();
  }

  while (isDigit(*curPtr) || *curPtr == '_') {
    ++curPtr;
  }

  auto tmpPtr = curPtr;
  if (AdvanceIfValidContinuationOfIdentifier(curPtr, bufferEnd)) {
    // diagnose(tmp, diag::lex_invalid_digit_in_fp_exponent, StringRef(tmp,
    // 1),
    //          false);
    return ExpectedDigit();
  }

  return MakeTok(tok::floating_literal, tokStart);
}

static const char *SkipToEndOfInterpolatedExpression(const char *curPtr,
                                                     const char *endPtr,
                                                     bool isMultilineString) {
  stone::Panic("Imp: SkipToEndOfInterpolatedExpression");
  return 0;
}

void Lexer::LexEscapedIdentifier() {
  stone::Panic("Impl: LexEscapedIdentifier");
}
void Lexer::LexStringLiteral(unsigned customDelimiterLen) {
  const char quoteChar = curPtr[-1];
  const char *tokStart = curPtr - 1 - customDelimiterLen;

  // NOTE: We only allow single-quote string literals so we can emit useful
  // diagnostics about changing them to double quotes.
  assert((quoteChar == '"' || quoteChar == '\'') && "Unexpected start");

  bool isMultilineString =
      AdvanceIfMultilineDelimiter(customDelimiterLen, curPtr, de, true);
  if (isMultilineString && *curPtr != '\n' && *curPtr != '\r') {
    stone::Panic("lex_illegal_multiline_string_start");
    // diagnose(curPtr, diag::lex_illegal_multiline_string_start)
    //     .fixItInsert(Lexer::getSourceLoc(curPtr), "\n");
  }

  bool wasErroneous = false;
  while (true) {
    // Handle string interpolation.
    const char *tmpPtr = curPtr + 1;
    if (*curPtr == '\\' &&
        DelimiterMatches(customDelimiterLen, tmpPtr, nullptr) &&
        *tmpPtr++ == '(') {
      // Consume tokens until we hit the corresponding ')'.
      curPtr = SkipToEndOfInterpolatedExpression(tmpPtr, bufferEnd,
                                                 isMultilineString);
      if (*curPtr == ')') {
        // Successfully scanned the body of the expression literal.
        ++curPtr;
        continue;
      } else if ((*curPtr == '\r' || *curPtr == '\n') && isMultilineString) {
        // The only case we reach here is unterminated single line string in the
        // interpolation. For better recovery, go on after emitting an error.
        stone::Panic(
            "lex_unterminated_string"); // diagnose(curPtr,
                                        // diag::lex_unterminated_string);
        wasErroneous = true;
        continue;
      } else {
        // diagnose(tokStart, diag::lex_unterminated_string);
        stone::Panic("lex_unterminated_string");
        return MakeTok(tok::alien, tokStart);
      }
    }
    // String literals cannot have \n or \r in them (unless multiline).
    if (((*curPtr == '\r' || *curPtr == '\n') && !isMultilineString) ||
        curPtr == bufferEnd) {
      stone::Panic("lex_unterminated_string");
      // diagnose(tokStart, diag::lex_unterminated_string);
      return MakeTok(tok::alien, tokStart);
    }

    unsigned charValue =
        LexChar(curPtr, quoteChar, true, isMultilineString, customDelimiterLen);
    // This is the end of string, we are done.
    if (charValue == ~0U) {
      break;
    }

    // Remember we had already-diagnosed invalid characters.
    wasErroneous |= charValue == ~1U;
  }
}
Lexer::NullCharKind Lexer::GetNullCharKind(const char *data) const {
  assert(data != nullptr && *data == 0);
  if (data == codeCompletionPtr) {
    return NullCharKind::CodeCompletion;
  }
  if (data == bufferEnd) {
    return NullCharKind::BufferEnd;
  }
  return NullCharKind::Embedded;
}

void Lexer::PrintD() {}

void Lexer::MakeTok(tok ty, const char *tokenStart) {
  assert((curPtr >= bufferStart) && (curPtr <= bufferEnd) &&
         "Cannot create token -- the current pointer is out of range!");
  // When we are lexing a subrange from the middle of a file buffer, we will
  // run past the end of the range, but will stay within the file.  Check if
  // we are past the imaginary EOF, and synthesize a tok::eof in this case.
  if (ty != tok::eof && tokenStart >= artificialEOF) {
    ty = tok::eof;
  }
  // TODO:
  unsigned commentLength = 0;

  llvm::StringRef tokenText{tokenStart,
                            static_cast<size_t>(curPtr - tokenStart)};

  if (triviaRetention == TriviaRetentionMode::With && ty != tok::eof) {
    assert(trailingTrivia.empty() && "TrailingTrivia is empty here");
    LexTrivia(trailingTrivia, /* IsForTrailingTrivia */ true);
  }
  nextToken.SetToken(ty, tokenText, commentLength);
  if (pipeline) {
    pipeline->OnToken(&nextToken);
  }
}

Token Lexer::GetTokenAtLoc(const SrcMgr &sm, SrcLoc loc) {
  // Don't try to do anything with an invalid location.
  if (!loc.isValid()) {
    return Token();
  }

  // Figure out which buffer contains this location.
  int bufferID = sm.findBufferContainingLoc(loc);
  if (bufferID < 0) {
    return Token();
  }

  // Use fake language options; language options only affect validity
  // and the exact token produced.
  SystemOptions fakeLangOpts;

  // Here we return comments as tokens because either the caller skipped
  // comments and normally we won't be at the beginning of a comment token
  // (making this option irrelevant), or the caller lexed comments and
  // we need to lex just the comment token.
  Lexer lexer(srcID, sm, nullptr, nullptr, pipeline);

  // TODO: lexer.RestoreState(LexerState(loc));
  return lexer.Peek();
}

SrcLoc Lexer::GetLocForEndOfToken(const SrcMgr &sm, SrcLoc loc) {
  return loc.getAdvancedLocOrInvalid(GetTokenAtLoc(sm, loc).GetLength());
}

InFlightDiagnostic Lexer::PrintD(const char *loc, Diagnostic diagnostic) {
  if (de){
    return de->PrintD(getSrcLoc(loc), diagnostic);
    }
    return InFlightDiagnostic();
  
}

void LexerStats::Print(ColorfulStream &stream) {}
