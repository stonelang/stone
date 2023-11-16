#include "stone/Parse/Parser.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/SyntaxResult.h"

SyntaxResult<Stmt> Parser::ParseStmt() {
  return MakeSyntaxResult<Stmt>(nullptr);
}
