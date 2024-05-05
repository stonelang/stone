#ifndef STONE_AST_DESCRIPTION_H
#define STONE_AST_DESCRIPTION_H

#include "llvm/ADT/PointerUnion.h"

namespace stone {
// TODO: SyntaxKind
enum class ASTDescription : uint8_t {
  Module,
  Decl,
  Stmt,
  Expr,
  QualType,
};

} // namespace stone

#endif
