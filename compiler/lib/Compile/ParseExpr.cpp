#include "stone/AST/Expr.h"
#include "stone/Compile/Parser.h"
#include "stone/Compile/ParserResult.h"

using namespace stone;

ParserResult<Expr> Parser::ParseExpr() {
  return stone::MakeParserResult<Expr>(nullptr);
}
