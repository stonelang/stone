#include "stone/AST/Expr.h"
#include "stone/CodeAna/Parser.h"
#include "stone/CodeAna/ParserResult.h"

using namespace stone;

ParserResult<Expr> Parser::ParseExpr() {
  return stone::MakeParserResult<Expr>(nullptr);
}
