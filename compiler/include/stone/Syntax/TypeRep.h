#ifndef STONE_SYNTAX_TYPEREP_H
#define STONE_SYNTAX_TYPEREP_H

namespace stone {

enum class TypeRepKind : uint8_t {
#define TYPEREP(ID, PARENT) ID,
#define LAST_TYPEREP(ID) LastTypeRep = ID,
#include "TypeRepKind.def"
};

} // namespace stone

#endif