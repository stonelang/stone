#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxResult.h"

using namespace stone::syn;

SyntaxResult<Expr *> Parser::ParseExpr() { return ExprEmpty(); }
