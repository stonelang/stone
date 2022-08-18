#ifndef STONE_SYNTAX_DESCRIPTION_H
#define STONE_SYNTAX_DESCRIPTION_H

#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {
// TODO: SyntaxKind
enum class SyntaxDescription : uint8_t {
  Module,
  Decl,
  Stmt,
  Expr,
  QualType,
};
} // namespace syn
} // namespace stone

#endif
