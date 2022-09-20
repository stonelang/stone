#include "stone/Basic/Defer.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"
#include "stone/Syntax/SyntaxNode.h"

using namespace stone;
using namespace stone::syn;

SyntaxStatus Parser::CollectDeclSpecifier(ParsingDeclCollector &collector) {}

SyntaxStatus Parser::CollectUsingSpecifier(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_using:
    collector.GetUsingDeclarationCollector().AddUsing(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectAccessLevel(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_public:
    collector.GetAccessLevelCollector().AddPublic(ConsumeToken());
    break;
  case tok::kw_internal:
    collector.GetAccessLevelCollector().AddInternal(ConsumeToken());
    break;
  case tok::kw_private:
    collector.GetAccessLevelCollector().AddPrivate(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectTypeQualifier(ParsingDeclCollector &collector) {
  //
}

SyntaxStatus Parser::CollectTypePatterns(ParsingDeclCollector &collector) {
  //
}
SyntaxStatus
Parser::CollectBasicTypeSpecifier(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_auto:
    collector.GetTypeSpecifierCollector().AddAuto(ConsumeToken());
    break;
  case tok::kw_int:
    collector.GetTypeSpecifierCollector().AddInt(ConsumeToken());
    break;
  case tok::kw_int8:
    collector.GetTypeSpecifierCollector().AddInt8(ConsumeToken());
    break;
  case tok::kw_int16:
    collector.GetTypeSpecifierCollector().AddInt16(ConsumeToken());
    break;
  case tok::kw_int32:
    collector.GetTypeSpecifierCollector().AddInt32(ConsumeToken());
    break;
  case tok::kw_int64:
    collector.GetTypeSpecifierCollector().AddInt64(ConsumeToken());
    break;
  case tok::kw_uint:
    collector.GetTypeSpecifierCollector().AddUInt(ConsumeToken());
    break;
  case tok::kw_uint8:
    collector.GetTypeSpecifierCollector().AddUInt8(ConsumeToken());
    break;
  case tok::kw_byte:
    collector.GetTypeSpecifierCollector().AddByte(ConsumeToken());
    break;
  case tok::kw_uint16:
    collector.GetTypeSpecifierCollector().AddUInt16(ConsumeToken());
    break;
  case tok::kw_uint32:
    collector.GetTypeSpecifierCollector().AddUInt32(ConsumeToken());
    break;
  case tok::kw_uint64:
    collector.GetTypeSpecifierCollector().AddUInt64(ConsumeToken());
    break;
  case tok::kw_float:
    collector.GetTypeSpecifierCollector().AddFloat(ConsumeToken());
    break;
  case tok::kw_float32:
    collector.GetTypeSpecifierCollector().AddFloat32(ConsumeToken());
    break;
  case tok::kw_float64:
    collector.GetTypeSpecifierCollector().AddFloat64(ConsumeToken());
    break;
  case tok::kw_complex32:
    collector.GetTypeSpecifierCollector().AddComplex32(ConsumeToken());
    break;
  case tok::kw_complex64:
    collector.GetTypeSpecifierCollector().AddComplex64(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus
Parser::CollectNominalTypeSpecifier(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_enum:
    collector.GetTypeSpecifierCollector().AddEnum(ConsumeToken());
    break;
  case tok::kw_struct:
    collector.GetTypeSpecifierCollector().AddStruct(ConsumeToken());
    break;
  case tok::kw_interface:
    collector.GetTypeSpecifierCollector().AddInterface(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectStorageSpecifier(ParsingDeclCollector &collector) {
  //
}
SyntaxStatus Parser::CollectFunctionSpecifier(ParsingDeclCollector &collector) {
  switch (GetCurTok().GetKind()) {
  case tok::kw_fun:
    collector.GetFunctionSpecifierCollector().AddFun(ConsumeToken());
    break;
  case tok::kw_inline:
    collector.GetFunctionSpecifierCollector().AddInline(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
