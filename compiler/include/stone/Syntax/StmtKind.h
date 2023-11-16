#ifndef STONE_SYNTAX_STMTKIND_H
#define STONE_SYNTAX_STMTKIND_H

namespace stone {

enum class StmtKind : uint8_t {

#define STMT(ID, PARENT) ID,
#define LAST_STMT(ID) Last_Stmt = ID,
#define STMT_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Stmt = FirstId, Last_##Id##Stmt = LastId,
#include "stone/Syntax/StmtKind.def"

#define EXPR(Id, Parent) Id,
#define LAST_EXPR(Id) Last_Expr = Id,
#define EXPR_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Expr = FirstId, Last_##Id##Expr = LastId,
#include "stone/Syntax/ExprKind.def"

};

} // namespace stone
#endif
