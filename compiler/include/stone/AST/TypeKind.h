#ifndef STONE_AST_TYPEKIND_H
#define STONE_AST_TYPEKIND_H

#include "stone/AST/InlineBitfield.h"

namespace stone {

enum class TypeKind : uint8_t {
  None = 0,
#define TYPE(id, parent) id,
#define LAST_TYPE(id) Last_Type = id,
#define TYPE_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/AST/TypeKind.def"
};

enum : uint8_t {
  NumTypeKindBits =
      stone::CountBitsUsed(static_cast<unsigned>(TypeKind::Last_Type))
};

enum class TypeSpecKind : uint8_t {
  None = 0,
#define TYPE_SPEC(ID, Parent) ID,
#include "stone/AST/TypeKind.def"
};

enum class TypeRepKind : uint8_t {
  None = 0,
#define ABSTRACT_TYPE(ID, Parent) ID,
#include "stone/AST/TypeKind.def"
};

enum class ScalarTypeKind {
  Pointer,
  BlockPointer,
  MemberPointer,
  Bool,
  Integral,
  Floating,
  IntegralComplex,
  FloatingComplex,
  FixedPoint
};

} // namespace stone
#endif