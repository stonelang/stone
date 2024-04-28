#include "stone/Parse/Parser.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/SyntaxResult.h"

using namespace stone;

SyntaxResult<Stmt> Parser::ParseStmt() {
  return MakeSyntaxResult<Stmt>(nullptr);
}
