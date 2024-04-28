#include "stone/AST/ASTContext.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/ParserResult.h"

using namespace stone;

ParserResult<Stmt> Parser::ParseStmt() {
  return MakeParserResult<Stmt>(nullptr);
}
