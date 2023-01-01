#include "stone/Syntax/QualType.h"

#ifndef STONE_SYNTAX_CANTYPE_H
#define STONE_SYNTAX_CANTYPE_H

namespace stone {
namespace syn {

class CanType final {
  QualType qualType;

public:
  CanType() = default;

public:
  explicit CanType(QualType qualType) : qualType(qualType) {}
};

} // namespace syn
} // namespace stone

#endif
