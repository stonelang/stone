#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxResult.h"

using namespace stone::syn;

SyntaxResult<Stmt> Parser::ParseStmt() {
  return syn::MakeSyntaxResult<Stmt>(nullptr);
}
