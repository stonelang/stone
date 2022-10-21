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

SyntaxStatus Parser::CollectDecl(ParsingDeclCollector &collector) {

  SyntaxStatus status;
  status = CollectUsingDecl(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectAccessLevel(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectFunctionDecl(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectBasicTypeDecl(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectNominalTypeDecl(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectTypeQualifier(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectStorageSpecifier(collector);
  if (status.IsSuccess()) {
    return status;
  }
  // If we are here, we did not find anything
  status.SetHasCodeCompletion();
  return status;
}

SyntaxStatus Parser::CollectUsingDecl(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_using:
    collector.GetUsingDeclarationCollector().AddUsing(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectAccessLevel(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
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

// TODO: Dulicate check
SyntaxStatus Parser::CollectTypeQualifier(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_const:
    collector.GetTypeCollector().GetTypeQualifierCollector().AddConst(
        ConsumeToken());
    break;
  case tok::kw_restrict:
    collector.GetTypeCollector().GetTypeQualifierCollector().AddRestrict(
        ConsumeToken());
    break;
  case tok::kw_volatile:
    collector.GetTypeCollector().GetTypeQualifierCollector().AddVolatile(
        ConsumeToken());
    break;
  case tok::kw_mutable:
    collector.GetTypeCollector().GetTypeQualifierCollector().AddMutable(
        ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}

bool Parser::IsTypeChunk(const Token &tk) {
  switch (tk.GetKind()) {
  case tok::star:
  case tok::amp:
    return true;
  default:
    return false;
  }
}

SyntaxStatus Parser::CollectTypeChunk(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::star:
    collector.GetTypeCollector().GetTypeChunkCollector().AddPointer(
        ConsumeToken());
    break;
  case tok::amp:
    collector.GetTypeCollector().GetTypeChunkCollector().AddReference(
        ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectTypeChunks(ParsingDeclCollector &collector) {

  assert(collector.GetTypeCollector().GetTypeSpecifierCollector().HasAny() &&
         "Attemping to collect type-patterns without a type");

  if (!GetTok().IsTypeChunk() && GetTok().IsIdentifierOrUnderscore()) {
    collector.GetTypeCollector().GetTypeChunkCollector().AddValue();
    return syn::MakeSyntaxSuccess();
  }
  // TODO: Simple for now but this will be greatly expanded
  SyntaxStatus status;
  while (GetTok().IsTypeChunk()) {
    status = CollectTypeChunk(collector);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
SyntaxStatus Parser::CollectBasicTypeDecl(ParsingDeclCollector &collector) {

  if (!GetTok().IsBasicType()) {
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  switch (GetTok().GetKind()) {
  // TODO: Think about void here
  case tok::kw_void:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddVoid(
        ConsumeToken());
    break;
  case tok::kw_auto:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddAuto(
        ConsumeToken());
    break;
  case tok::kw_int:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddInt(
        ConsumeToken());
    break;
  case tok::kw_int8:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddInt8(
        ConsumeToken());
    break;
  case tok::kw_int16:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddInt16(
        ConsumeToken());
    break;
  case tok::kw_int32:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddInt32(
        ConsumeToken());
    break;
  case tok::kw_int64:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddInt64(
        ConsumeToken());
    break;
  case tok::kw_uint:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddUInt(
        ConsumeToken());
    break;
  case tok::kw_uint8:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddUInt8(
        ConsumeToken());
    break;
  case tok::kw_byte:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddByte(
        ConsumeToken());
    break;
  case tok::kw_uint16:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddUInt16(
        ConsumeToken());
    break;
  case tok::kw_uint32:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddUInt32(
        ConsumeToken());
    break;
  case tok::kw_uint64:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddUInt64(
        ConsumeToken());
    break;
  case tok::kw_float:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddFloat(
        ConsumeToken());
    break;
  case tok::kw_float32:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddFloat32(
        ConsumeToken());
    break;
  case tok::kw_float64:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddFloat64(
        ConsumeToken());
    break;
  case tok::kw_complex32:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddComplex32(
        ConsumeToken());
    break;
  case tok::kw_complex64:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddComplex64(
        ConsumeToken());
    break;
  case tok::kw_imaginary32:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddImaginary32(
        ConsumeToken());
  case tok::kw_imaginary64:
    break;
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddImaginary64(
        ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectNominalTypeDecl(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_enum:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddEnum(
        ConsumeToken());
    break;
  case tok::kw_struct:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddStruct(
        ConsumeToken());
    break;
  case tok::kw_interface:
    collector.GetTypeCollector().GetTypeSpecifierCollector().AddInterface(
        ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectStorageSpecifier(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_static:
    collector.GetStorageSpecifierCollector().AddStatic(ConsumeToken());
    break;
  case tok::kw_register:
    collector.GetStorageSpecifierCollector().AddRegister(ConsumeToken());
    break;
  default:
    return syn::MakeSyntaxCodeCompletionStatus();
  }
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectFunctionDecl(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
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

SyntaxStatus Parser::VerifyDeclCollected(ParsingDeclCollector &collector) {
  return syn::MakeSyntaxSuccess();
}
