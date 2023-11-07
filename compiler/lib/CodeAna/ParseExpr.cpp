#include "stone/AST/ParserResult.h"
#include "stone/CodeAna/Parser.h"

using namespace stone::ast;

ParserResult<Expr> Parser::ParseExpr() {
  return parser::MakeParserResult<Expr>(nullptr);
}
