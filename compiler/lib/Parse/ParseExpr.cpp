#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxResult.h"

SyntaxResult<Expr> Parser::ParseExpr() {
  return MakeSyntaxResult<Expr>(nullptr);
}
