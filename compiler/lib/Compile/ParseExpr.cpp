#include "stone/AST/ParserResult.h"
#include "stone/Compile/Parser.h"

using namespace stone::ast;

ParserResult<Expr> Parser::ParseExpr() {
  return parser::MakeParserResult<Expr>(nullptr);
}
