#ifndef STONE_SYNTAX_STMTKIND_H
#define STONE_SYNTAX_STMTKIND_H

namespace stone {
namespace syn {

enum class ExprKind : UInt8 {
#define EXPR(ID, PARENT) ID,
#define LAST_STMT(ID) Last_Stmt = ID,
#define STMT_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Stmt = FirstId, Last_##Id##Stmt = LastId,
#include "stone/Syntax/StmtKind.def"
};
} // namespace syn
} // namespace stone
#endif
