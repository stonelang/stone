#ifndef STONE_SYNTAX_EXPRFACTORY_H
#define STONE_SYNTAX_EXPRFACTORY_H

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {

class Expr;
struct ExprFactory final {
  NewExpr *MakeNewExpr();
};

} // namespace stone

#endif
