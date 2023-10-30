#include "stone/Parse/Parser.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/ParserResult.h"

using namespace stone::ast;

ParserResult<Stmt> Parser::ParseStmt() {
  return parser::MakeParserResult<Stmt>(nullptr);
}
