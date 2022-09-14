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
SyntaxResult<QualType> Parser::ParseType(TypeSpecifierCollector &collector,
                                         Diag<> diagID) {

  SyntaxResult<QualType> result;

  ParsingScope parsingType(*this, ScopeKind::Type, "parsing type");

  result = ParseBasicType(collector, diagID);
  return result;
}

SyntaxResult<QualType>
Parser::ParseDeclResultType(TypeSpecifierCollector &collector, Diag<> diagID) {

  // assert(curTok.IsIdentifierOrUnderscore)

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
SyntaxStatus
Parser::ParseBasicTypeSpecifier(TypeSpecifierCollector &collector) {
  SyntaxStatus status;

  // We can only collect one
  if (collector.HasTypeSpecifierKind()) {
    status.SetIsError();
    return status;
  }
  switch (curTok.GetKind()) {
  case tok::kw_auto:
    collector.AddAuto(ConsumeToken());
    break;
  case tok::kw_int:
    collector.AddInt(ConsumeToken());
    break;
  case tok::kw_int8:
    collector.AddInt8(ConsumeToken());
    break;
  case tok::kw_int16:
    collector.AddInt16(ConsumeToken());
    break;
  case tok::kw_int32:
    collector.AddInt32(ConsumeToken());
    break;
  case tok::kw_int64:
    collector.AddInt64(ConsumeToken());
    break;
  case tok::kw_uint:
    collector.AddUInt(ConsumeToken());
    break;
  case tok::kw_uint8:
    collector.AddUInt8(ConsumeToken());
    break;
  case tok::kw_byte:
    collector.AddByte(ConsumeToken());
    break;
  case tok::kw_uint16:
    collector.AddUInt16(ConsumeToken());
    break;
  case tok::kw_uint32:
    collector.AddUInt32(ConsumeToken());
    break;
  case tok::kw_uint64:
    collector.AddUInt64(ConsumeToken());
    break;
  case tok::kw_float:
    collector.AddFloat(ConsumeToken());
    break;
  case tok::kw_float32:
    collector.AddFloat32(ConsumeToken());
    break;
  case tok::kw_float64:
    collector.AddFloat64(ConsumeToken());
    break;
  case tok::kw_complex32:
    collector.AddComplex32(ConsumeToken());
    break;
  case tok::kw_complex64:
    collector.AddComplex64(ConsumeToken());
    break;
  default:
    return status;
  }
  status.SetHasCodeCompletion();
  return status;
}

SyntaxStatus Parser::ParseTypeQualifiers(TypeQualifierCollector &collector) {
  SyntaxStatus status;
  switch (curTok.GetKind()) {
  case tok::kw_const:
    if (!collector.HasConst()) {
      collector.AddConst(ConsumeToken());
    } else {
      /// Log -- attempting to add a const
    }
    // We do not consume the token because the QualType that we create
    // will be of the following const int i = ....
    break;
  case tok::kw_volatile:
    if (!collector.HasVolatile()) {
      collector.AddVolatile(ConsumeToken());
    } else {
      // Log
    }
    break;
  case tok::kw_restrict:
    if (!collector.HasRestrict()) {
      collector.AddRestrict(ConsumeToken());
    } else {
      // Log
    }
    break;
  case tok::kw_pure:
    if (!collector.HasPure()) {
      collector.AddPure(ConsumeToken());
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
