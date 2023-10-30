#include "stone/AST/QualType.h"

#ifndef STONE_AST_CANTYPE_H
#define STONE_AST_CANTYPE_H

namespace stone {
namespace ast {

class CanType final {
  QualType qualType;

public:
  CanType() = default;

public:
  explicit CanType(QualType qualType) : qualType(qualType) {}
};

} // namespace ast
} // namespace stone

#endif
