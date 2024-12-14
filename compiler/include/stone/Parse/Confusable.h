#ifndef STONE_PARSE_CONFUSABLE_H
#define STONE_PARSE_CONFUSABLE_H

#include "llvm/ADT/StringRef.h"

#include <stdint.h>

namespace stone {
/// Given a UTF-8 codepoint, determines whether it appears on the Unicode
/// specification table of confusable characters and maps to punctuation,
/// and either returns either the expected ASCII character or 0.
char ConvertConfusableCharacterToASCII(uint32_t codepoint);

std::pair<llvm::StringRef, llvm::StringRef>
GetConfusableAndBaseCodepointNames(uint32_t codepoint);

} // namespace stone

#endif
