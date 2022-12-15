#ifndef STONE_SYNTAX_EXPRFACTORY_H
#define STONE_SYNTAX_EXPRFACTORY_H

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {
class Expr;
} // namespace syn

namespace syn {
struct ExprFactory final {
    NewExpr* MakeNewExpr();
};

} // namespace syn
} // namespace stone

#endif
