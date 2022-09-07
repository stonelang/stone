#ifndef STONE_SYNTAX_TYPEKIND_H
#define STONE_SYNTAX_TYPEKIND_H

#include "stone/Basic/STDTypeAlias.h"
#include "stone/Syntax/InlineBitfield.h"

namespace stone {
namespace syn {

enum class TypeKind : UInt8 {
#define TYPE(id, parent) id,
#define LAST_TYPE(id) LastType = id,
#define TYPE_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/Syntax/TypeKind.def"
};

enum : unsigned {
  NumTypeKindBits =
      stone::CountBitsUsed(static_cast<unsigned>(TypeKind::LastType))
};

} // namespace syn
} // namespace stone
#endif
