#include "stone/Parse/Parser.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTResult.h"

using namespace stone::ast;

ASTResult<Stmt> Parser::ParseStmt() {
  return ast::MakeASTResult<Stmt>(nullptr);
}
