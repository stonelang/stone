#ifndef STONE_PARSE_CONFUSABLES_H
#define STONE_PARSE_CONFUSABLES_H

#include <stdint.h>

namespace stone {
namespace syn {
/// Given a UTF-8 codepoint, determines whether it appears on the Unicode
/// specification table of confusable characters and maps to punctuation,
/// and either returns either the expected ASCII character or 0.
char ConvertConfusableCharacterToASCII(uint32_t codepoint);
} // namespace syn
} // namespace stone

#endif
