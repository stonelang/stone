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
  case tok::kw_complex32:
  case tok::kw_complex64:
    return true;
  default:
    return false;
  }
}

// Similar to ParseDeclSpecifiers
SyntaxResult<QualType> Parser::ParseType(TypeSpecifierContext &specContext) {
  return syn::MakeSyntaxResult<QualType>(nullptr);
}

SyntaxResult<QualType>
Parser::ParseDeclResultType(TypeSpecifierContext &specContext, Diag<> diagID) {
  return ParseType(specContext);
}

SyntaxResult<QualType> Parser::ParseBasicType(TypeSpecifierContext &specContext,
                                              Diag<> diagID) {

  assert(IsBasicType(curTok.GetKind()) &&
         "The current token is not a basic type");

  /// Maybe parse ParseDeclarator -- or just take a look at the code
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

llvm::Optional<bool>
Parser::ParseBasicTypeSpecifier(TypeSpecifierContext &specifier) {
  SrcLoc loc = curTok.GetLoc();
  switch (curTok.GetKind()) {
  case tok::kw_int8:
    specifier.AddInt8(loc);
    break;
  case tok::kw_int16:
    specifier.AddInt16(loc);
    break;
  case tok::kw_int32:
    specifier.AddInt32(loc);
    break;
  case tok::kw_int64:
    specifier.AddInt64(loc);
    break;
  case tok::kw_int:
    specifier.AddInt(loc);
    break;
  case tok::kw_uint:
    specifier.AddUInt(loc);
    break;
  case tok::kw_uint8:
    specifier.AddUInt8(loc);
    break;
  case tok::kw_byte:
    specifier.AddByte(loc);
    break;
  case tok::kw_uint16:
    specifier.AddUInt16(loc);
    break;
  case tok::kw_uint32:
    specifier.AddUInt32(loc);
    break;
  case tok::kw_uint64:
    specifier.AddUInt64(loc);
    break;
  case tok::kw_float:
    specifier.AddFloat(loc);
  case tok::kw_float32:
    specifier.AddFloat32(loc);
    break;
  case tok::kw_float64:
    specifier.AddFloat64(loc);
    break;
  case tok::kw_complex32:
    specifier.AddComplex32(loc);
    break;
  case tok::kw_complex64:
    specifier.AddComplex64(loc);
    break;
    break;
  default:
    return llvm::None;
    break;
  }
Found:
  return true;
}
