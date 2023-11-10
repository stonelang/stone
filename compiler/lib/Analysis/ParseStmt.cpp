#include "stone/AST/ASTContext.h"
#include "stone/Analysis/Parser.h"
#include "stone/Analysis/ParserResult.h"

using namespace stone;

ParserResult<Stmt> Parser::ParseStmt() {
  return stone::MakeParserResult<Stmt>(nullptr);
}
