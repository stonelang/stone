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

enum class BuiltinTypeKind : std::underlying_type<TypeKind>::type {
#define TYPE(id, parent)
#define BUILTIN_TYPE(id, parent)                                               \
  id = std::underlying_type<TypeKind>::type(TypeKind::id),
#include "stone/AST/TypeKind.def"
};


} // namespace stone
#endif