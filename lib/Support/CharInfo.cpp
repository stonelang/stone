#include "stone/Support/CharInfo.h"
#include "clang/Basic/CharInfo.h"

#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/MemoryBuffer.h"
using namespace stone;

/// EncodeToUTF8 - Encode the specified code point into a UTF8 stream.  Return
/// true if it is an erroneous code point.
bool stone::charinfo::EncodeToUTF8(unsigned CharValue,
                                   SmallVectorImpl<char> &Result) {
  // Number of bits in the value, ignoring leading zeros.
  unsigned NumBits = 32 - llvm::countl_zero(CharValue);

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

/// stone::charinfo::CLO8 - Return the number of leading ones in the specified
/// 8-bit value.
unsigned stone::charinfo::CLO8(unsigned char C) {
  return llvm::countl_one(uint32_t(C) << 24);
}

/// isStartOfUTF8Character - Return true if this isn't a UTF8 continuation
/// character, which will be of the form 0b10XXXXXX
bool stone::charinfo::IsStartOfUTF8Character(unsigned char C) {
  // RFC 2279: The octet values FE and FF never appear.
  // RFC 3629: The octet values C0, C1, F5 to FF never appear.
  return C <= 0x80 || (C >= 0xC2 && C < 0xF5);
}

/// validateUTF8CharacterAndAdvance - Given a pointer to the starting byte of a
/// UTF8 character, validate it and advance the lexer past it.  This returns the
/// encoded character or ~0U if the encoding is invalid.
uint32_t stone::charinfo::ValidateUTF8CharacterAndAdvance(const char *&Ptr,
                                                          const char *End) {
  if (Ptr >= End)
    return ~0U;

  unsigned char CurByte = *Ptr++;
  if (CurByte < 0x80)
    return CurByte;

  // Read the number of high bits set, which indicates the number of bytes in
  // the character.
  unsigned EncodedBytes = stone::charinfo::CLO8(CurByte);

  // If this is 0b10XXXXXX, then it is a continuation character.
  if (EncodedBytes == 1 || !stone::charinfo::IsStartOfUTF8Character(CurByte)) {
    // Skip until we get the start of another character.  This is guaranteed to
    // at least stop at the nul at the end of the buffer.
    while (Ptr < End && !stone::charinfo::IsStartOfUTF8Character(*Ptr))
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
  unsigned NumBits = 32 - llvm::countl_one(CharValue);

  if (NumBits <= 5 + 6)
    return EncodedBytes == 2 ? CharValue : ~0U;
  if (NumBits <= 4 + 6 + 6)
    return EncodedBytes == 3 ? CharValue : ~0U;
  return EncodedBytes == 4 ? CharValue : ~0U;
}

bool stone::charinfo::IsValidIdentifierContinuationCodePoint(uint32_t c) {

  /// TODO:
  // if (c < 0x80) {
  //   return clang::isAsciiIdentifierContinue(c, /*dollar*/ true);
  // }

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
