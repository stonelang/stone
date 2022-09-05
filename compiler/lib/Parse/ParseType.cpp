#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Types.h"

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

  SyntaxResult<QualType> result;
  return result;
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

SyntaxStatus Parser::ParseBasicTypeSpecifier(TypeSpecifierContext &specifier) {
  SyntaxStatus status;
  switch (curTok.GetKind()) {
  case tok::kw_auto:
    specifier.AddAuto(ConsumeToken());
    break;
  case tok::kw_int:
    specifier.AddInt(ConsumeToken());
    break;
  case tok::kw_int8:
    specifier.AddInt8(ConsumeToken());
    break;
  case tok::kw_int16:
    specifier.AddInt16(ConsumeToken());
    break;
  case tok::kw_int32:
    specifier.AddInt32(ConsumeToken());
    break;
  case tok::kw_int64:
    specifier.AddInt64(ConsumeToken());
    break;
  case tok::kw_uint:
    specifier.AddUInt(ConsumeToken());
    break;
  case tok::kw_uint8:
    specifier.AddUInt8(ConsumeToken());
    break;
  case tok::kw_byte:
    specifier.AddByte(ConsumeToken());
    break;
  case tok::kw_uint16:
    specifier.AddUInt16(ConsumeToken());
    break;
  case tok::kw_uint32:
    specifier.AddUInt32(ConsumeToken());
    break;
  case tok::kw_uint64:
    specifier.AddUInt64(ConsumeToken());
    break;
  case tok::kw_float:
    specifier.AddFloat(ConsumeToken());
    break;
  case tok::kw_float32:
    specifier.AddFloat32(ConsumeToken());
    break;
  case tok::kw_float64:
    specifier.AddFloat64(ConsumeToken());
    break;
  case tok::kw_complex32:
    specifier.AddComplex32(ConsumeToken());
    break;
  case tok::kw_complex64:
    specifier.AddComplex64(ConsumeToken());
    break;
  default:
    return status;
  }
  status.SetHasCodeCompletion();
  return status;
}

SyntaxStatus Parser::ParseTypeQualifiers(TypeQualifierContext &qualContext) {
  SyntaxStatus status;
  switch (curTok.GetKind()) {
  case tok::kw_const:
    if (!qualContext.HasConst()) {
      qualContext.AddConst(ConsumeToken());
    } else {
      /// Log -- attempting to add a const
    }
    // We do not consume the token because the QualType that we create
    // will be of the following const int i = ....
    break;
  case tok::kw_volatile:
    if (!qualContext.HasVolatile()) {
      qualContext.AddVolatile(ConsumeToken());
    } else {
      // Log
    }
    break;
  case tok::kw_restrict:
    if (!qualContext.HasRestrict()) {
      qualContext.AddRestrict(ConsumeToken());
    } else {
      // Log
    }
    break;
  case tok::kw_pure:
    if (!qualContext.HasPure()) {
      qualContext.AddPure(ConsumeToken());
    } else {
      // Log
    }
    break;
  default:
    return status;
  }
  status.SetHasCodeCompletion();
  return status;
}
