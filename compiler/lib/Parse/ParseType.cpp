#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

// Similar to ParseDeclSpecifiers
SyntaxResult<QualType> Parser::ParseType(TypeSpecifierCollector &collector,
                                         Diag<> diagID) {

  SyntaxResult<QualType> result;

  ParsingScope parsingType(*this, ScopeKind::Type, "parsing type");

  result = ParseBasicType(collector, diagID);
  return result;
}

SyntaxResult<QualType>
Parser::ParseDeclResultType(TypeSpecifierCollector &collector, Diag<> diagID) {

  return ParseType(collector, diagID);
}

SyntaxResult<QualType> Parser::ParseBasicType(TypeSpecifierCollector &collector,
                                              Diag<> diagID) {

  SyntaxResult<QualType> result;
  assert(IsBasicType(curTok.GetKind()) &&
         "The current token is not a basic type");

  /// Maybe parse ParseDeclarator -- or just take a look at the code
  switch (curTok.GetKind()) {
  case tok::identifier: {
    ParseIdentifierType(collector, diagID);
    break;
  }
  default:
    break;
  }
  result;
}

void Parser::ParseIdentifierType(TypeSpecifierCollector &collector,
                                 Diag<> diagID) {}
