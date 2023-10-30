#ifndef STONE_ASTEXPRKIND_H
#define STONE_ASTEXPRKIND_H

#include "stone/AST/InlineBitfield.h"

namespace stone {
namespace ast {

enum class ExprKind : uint8_t {
#define EXPR(Id, Parent) Id,
#define LAST_EXPR(Id) Last_Expr = Id,
#define EXPR_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Expr = FirstId, Last_##Id##Expr = LastId,
#include "stone/AST/ExprKind.def"
};
enum : unsigned {
  NumExprKindBits =
      stone::CountBitsUsed(static_cast<unsigned>(ExprKind::Last_Expr))
};

} // namespace ast
} // namespace stone
#endif
