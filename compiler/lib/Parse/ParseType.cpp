#include "stone/Parse/Parser.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Type.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsBasicType(Token &anyTok) const {

  if (!anyTok.IsKeyword()) {
    return false;
  }
  // TODO: A switch may be better for perf?
  return anyTok.IsAny(tok::kw_int8, tok::kw_int16, tok::kw_int32, tok::kw_int64,
                      tok::kw_int, tok::kw_uint, tok::kw_uint8, tok::kw_byte,
                      tok::kw_ubyte, tok::kw_uint16, tok::kw_uint32,
                      tok::kw_uint64, tok::kw_float32, tok::kw_float64);
}

// Similar to ParseDeclSpecifiers
SyntaxResult<QualType> Parser::ParseType() {
  return syn::MakeSyntaxResult<QualType>(nullptr);
}

SyntaxResult<QualType> Parser::ParseDeclResultType(Diag<> diagID) {
  return ParseType();
}

SyntaxResult<QualType> Parser::ParseBasicType(Diag<> diagID) {

  switch (curTok.GetKind()) {
  case tok::identifier: {
    // ty = ParseIdentifierType();
    break;
  }
  default:
    break;
  }
  return syn::MakeSyntaxResult<QualType>(nullptr);
}
