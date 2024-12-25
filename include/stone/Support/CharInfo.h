#ifndef STONE_SUPPORT_CHARINFO_H
#define STONE_SUPPORT_CHARINFO_H

#include "stone/Basic/LLVM.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"

namespace stone {
namespace charinfo {
/// EncodeToUTF8 - Encode the specified code point into a UTF8 stream.  Return
/// true if it is an erroneous code point.
bool EncodeToUTF8(unsigned CharValue, SmallVectorImpl<char> &Result);

/// CLO8 - Return the number of leading ones in the specified 8-bit value.
unsigned CLO8(unsigned char C);

/// isStartOfUTF8Character - Return true if this isn't a UTF8 continuation
/// character, which will be of the form 0b10XXXXXX
bool IsStartOfUTF8Character(unsigned char C);

/// validateUTF8CharacterAndAdvance - Given a pointer to the starting byte of a
/// UTF8 character, validate it and advance the lexer past it.  This returns the
/// encoded character or ~0U if the encoding is invalid.
uint32_t ValidateUTF8CharacterAndAdvance(const char *&Ptr, const char *End);

bool IsValidIdentifierContinuationCodePoint(uint32_t c);
} // namespace charinfo

} // namespace stone
#endif
