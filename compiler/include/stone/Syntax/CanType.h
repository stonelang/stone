#include "stone/Syntax/QualType.h"

#ifndef STONE_SYNTAX_CANTYPE_H
#define STONE_SYNTAX_CANTYPE_H

namespace stone {
namespace syn {

class CanType final {

  // QualType qulType;
public:
  CanType() = default;

public:
  // explicit CanType(Type *ty = 0) : Type(ty) {
  //   assert(IsCanTypeOrNull() &&
  //          "Forming a CanType out of a non-canonical type!");
  // }
  // explicit CanType(QualType ty) : Type(ty) {
  //   assert(IsCanTypeOrNull() &&
  //          "Forming a CanType out of a non-canonical type!");
  // }
};

} // namespace syn
} // namespace stone

#endif