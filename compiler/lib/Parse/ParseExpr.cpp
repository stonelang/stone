#include "stone/Parse/Parser.h"
#include "stone/Parse/ParserResult.h"

using namespace stone;

ParserResult<Expr> Parser::ParseExpr() {
  return MakeParserResult<Expr>(nullptr);
}
