#ifndef STONE_AST_TYPEKIND_H
#define STONE_AST_TYPEKIND_H

#include "stone/Basic/STDAlias.h"
#include "stone/AST/InlineBitfield.h"

namespace stone {

enum class TypeKind : UInt8 {
#define TYPE(id, parent) id,
#define LAST_TYPE(id) Last_Type = id,
#define TYPE_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/AST/TypeKind.def"
};

enum : unsigned {
  NumTypeKindBits =
      stone::CountBitsUsed(static_cast<unsigned>(TypeKind::Last_Type))
};

} // namespace stone
#endif
