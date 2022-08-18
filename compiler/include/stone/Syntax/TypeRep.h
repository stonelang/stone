#ifndef STONE_SYNTAX_TYPEREP_H
#define STONE_SYNTAX_TYPEREP_H

namespace stone {
namespace syn {
class QualType;

// class Qualifier;

enum class TypeRepKind : uint8_t {
#define TYPEREP(ID, PARENT) ID,
#define LAST_TYPEREP(ID) LastTypeRep = ID,
#include "TypeRepKind.def"
};

// class TypeRep {
// 	Qualifier qualifier;

// };

} // namespace syn

} // namespace stone

#endif