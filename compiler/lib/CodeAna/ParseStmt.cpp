#include "stone/AST/ASTContext.h"
#include "stone/CodeAna/Parser.h"
#include "stone/CodeAna/ParserResult.h"

using namespace stone;

ParserResult<Stmt> Parser::ParseStmt() {
  return stone::MakeParserResult<Stmt>(nullptr);
}
