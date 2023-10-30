#include "stone/Parse/Parser.h"
#include "stone/AST/ASTResult.h"

using namespace stone::ast;

ASTResult<Expr> Parser::ParseExpr() {
  return ast::MakeASTResult<Expr>(nullptr);
}
