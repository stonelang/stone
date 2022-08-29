#include "stone/Parse/Parser.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Type.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsBasicType(tok kind) const {
  // TODO: A switch may be better for perf?
  switch (kind) {
  case tok::kw_int8:
  case tok::kw_int16:
  case tok::kw_int32:
  case tok::kw_int64:
  case tok::kw_int:
  case tok::kw_uint:
  case tok::kw_uint8:
  case tok::kw_byte:
  case tok::kw_ubyte:
  case tok::kw_uint16:
  case tok::kw_uint32:
  case tok::kw_uint64:
  case tok::kw_float32:
  case tok::kw_float64:
    return true;
  default:
    return false;
  }
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
