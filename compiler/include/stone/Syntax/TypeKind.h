#ifndef STONE_SYNTAX_TYPEKIND_H
#define STONE_SYNTAX_TYPEKIND_H

namespace stone {
namespace syn {

enum class TypeKind : uint8_t {
#define TYPE(id, parent) id,
#define LAST_TYPE(id) Last_Type = id,
#define TYPE_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/Syntax/TypeKind.def"
};

} // namespace syn
} // namespace stone
#endif
