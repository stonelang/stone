#include "stone/AST/QualType.h"

#ifndef STONE_ASTCANTYPE_H
#define STONE_ASTCANTYPE_H

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
