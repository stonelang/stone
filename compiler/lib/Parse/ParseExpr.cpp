#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxResult.h"

using namespace stone;

SyntaxResult<Expr> Parser::ParseExpr() {
  return MakeSyntaxResult<Expr>(nullptr);
}
