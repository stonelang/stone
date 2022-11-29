#ifndef STONE_SYNTAX_EXPRKIND_H
#define STONE_SYNTAX_EXPRKIND_H

#include "stone/Syntax/InlineBitfield.h"

namespace stone {
namespace syn {

enum class ExprKind : uint8_t {
#define EXPR(Id, Parent) Id,
#define LAST_EXPR(Id) Last_Expr = Id,
#define EXPR_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Expr = FirstId, Last_##Id##Expr = LastId,
#include "stone/Syntax/ExprKind.def"
};
enum : unsigned {
  NumExprKindBits =
      stone::CountBitsUsed(static_cast<unsigned>(ExprKind::Last_Expr))
};

} // namespace syn
} // namespace stone
#endif
