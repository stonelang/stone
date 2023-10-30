#include "stone/Parse/Parser.h"
#include "stone/AST/ParserResult.h"

using namespace stone::ast;

ParserResult<Expr> Parser::ParseExpr() {
  return parser::MakeParserResult<Expr>(nullptr);
}
