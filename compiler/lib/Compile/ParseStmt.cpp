#include "stone/Parse/Parser.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTResult.h"

using namespace stone::syn;

ASTResult<Stmt> Parser::ParseStmt() {
  return syn::MakeASTResult<Stmt>(nullptr);
}
