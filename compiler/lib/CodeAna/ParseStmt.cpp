#include "stone/AST/ASTContext.h"
#include "stone/CodeAna/Parser.h"
#include "stone/CodeAna/ParserResult.h"

using namespace stone::ast;

ParserResult<Stmt> Parser::ParseStmt() {
  return codeana::MakeParserResult<Stmt>(nullptr);
}
