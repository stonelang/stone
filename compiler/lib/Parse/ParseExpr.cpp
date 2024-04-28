#include "stone/Parse/Parser.h"
#include "stone/AST/SyntaxResult.h"

using namespace stone;

SyntaxResult<Expr> Parser::ParseExpr() {
  return MakeSyntaxResult<Expr>(nullptr);
}
