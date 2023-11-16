#include "stone/Parse/Parser.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/SyntaxResult.h"


using namespace stone;

SyntaxResult<Stmt> Parser::ParseStmt() {
  return MakeSyntaxResult<Stmt>(nullptr);
}
