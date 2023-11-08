#include "stone/AST/Expr.h"
#include "stone/CodeAna/ParserResult.h"
#include "stone/CodeAna/Parser.h"

using namespace stone::ast;
using namespace stone::codeana;

ParserResult<Expr> Parser::ParseExpr() {
  return codeana::MakeParserResult<Expr>(nullptr);
}
