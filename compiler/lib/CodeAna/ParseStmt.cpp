#include "stone/AST/ASTContext.h"
#include "stone/AST/ParserResult.h"
#include "stone/CodeAna/Parser.h"

using namespace stone::ast;

ParserResult<Stmt> Parser::ParseStmt() {
  return codeana::MakeParserResult<Stmt>(nullptr);
}
