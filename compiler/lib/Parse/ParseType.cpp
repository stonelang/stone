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

  switch (curTok.GetKind()) {
  case tok::kw_int8:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Int8, curTok.GetLoc());
    break;
  case tok::kw_int16:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Int16, curTok.GetLoc());
    break;
  case tok::kw_int32:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Int32, curTok.GetLoc());
    goto Found;
  case tok::kw_int64:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Int64, curTok.GetLoc());
    break;
  case tok::kw_int:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Int, curTok.GetLoc());
    break;
  case tok::kw_uint:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::UInt, curTok.GetLoc());
    break;
  case tok::kw_uint8:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::UInt8, curTok.GetLoc());
    break;
  case tok::kw_byte:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Byte, curTok.GetLoc());
    break;
  case tok::kw_uint16:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::UInt16, curTok.GetLoc());
    break;
  case tok::kw_uint32:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::UInt32, curTok.GetLoc());
    break;
  case tok::kw_uint64:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::UInt64, curTok.GetLoc());
    break;
  case tok::kw_float:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Float, curTok.GetLoc());
  case tok::kw_float32:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Float32, curTok.GetLoc());
    break;
  case tok::kw_float64:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Float64, curTok.GetLoc());
    break;
  case tok::kw_complex32:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Complex32,
                                   curTok.GetLoc());
    break;
  case tok::kw_complex64:
    specifier.SetTypeSpecifierKind(TypeSpecifierKind::Complex64,
                                   curTok.GetLoc());
    break;
  default:
    return llvm::None;
    break;
  }
Found:
  return true;
}
