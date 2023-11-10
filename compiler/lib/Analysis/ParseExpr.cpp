#include "stone/AST/Expr.h"
#include "stone/Analysis/Parser.h"
#include "stone/Analysis/ParserResult.h"

using namespace stone;

ParserResult<Expr> Parser::ParseExpr() {
  return stone::MakeParserResult<Expr>(nullptr);
}
