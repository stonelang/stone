#include "stone/Parse/Parser.h"
#include "stone/AST/ASTResult.h"

using namespace stone::syn;

ASTResult<Expr> Parser::ParseExpr() {
  return syn::MakeASTResult<Expr>(nullptr);
}
