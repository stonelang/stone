#include "stone/AST/ASTContext.h"
#include "stone/Compile/Parser.h"
#include "stone/Compile/ParserResult.h"

using namespace stone;

ParserResult<Stmt> Parser::ParseStmt() {
  return stone::MakeParserResult<Stmt>(nullptr);
}
