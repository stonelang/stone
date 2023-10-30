#ifndef STONE_AST_EXPRFACTORY_H
#define STONE_AST_EXPRFACTORY_H

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace ast {
class Expr;
} // namespace ast

namespace ast {
struct ExprFactory final {
  NewExpr *MakeNewExpr();
};

} // namespace ast
} // namespace stone

#endif
