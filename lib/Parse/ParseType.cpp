#include "stone/Parse/Parser.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxBuilder.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Type.h"

using namespace stone;
using namespace stone::syn;

// Similar to ParseDeclSpecifiers
SyntaxResult<QualType> Parser::ParseType() {
  return syn::MakeSyntaxResult<QualType>(nullptr);
}

SyntaxResult<QualType> Parser::ParseDeclResultType(Diag<> diagID) {
  return ParseType();
}