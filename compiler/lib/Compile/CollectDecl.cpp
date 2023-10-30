#include "stone/Basic/Defer.h"
#include "stone/Diag/ASTDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/AST/DeclFactory.h"
#include "stone/AST/Stmt.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTNode.h"

using namespace stone;
using namespace stone::syn;

ASTStatus Parser::CollectDecl(ParsingDeclCollector &collector) {

  ASTStatus status;
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
  status = CollectBasicTypeDecl(collector.GetTypeCollector());
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectNominalTypeDecl(collector.GetTypeCollector());
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectTypeQualifier(collector.GetTypeCollector());
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

ASTStatus Parser::CollectUsingDecl(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_using:
    collector.GetUsingDeclarationCollector().AddUsing(ConsumeToken());
    break;
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}

ASTStatus Parser::CollectTypeOperator(TypeCollector &collector) {
  // if(collector.GetTypeOperatorCollector().HasAny()){
  // }
  switch (GetTok().GetKind()) {
  case tok::kw_new:
    collector.GetTypeOperatorCollector().AddNew(ConsumeToken());
    break;
  case tok::kw_delete:
    collector.GetTypeOperatorCollector().AddDelete(ConsumeToken());
    break;
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}

ASTStatus Parser::CollectAccessLevel(ParsingDeclCollector &collector) {
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
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}

// TODO: Dulicate check
ASTStatus Parser::CollectTypeQualifiers(TypeCollector &collector) {
  ASTStatus status;
  while (GetTok().IsQualifier()) {
    status = CollectTypeQualifier(collector);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
// TODO: Dulicate check
ASTStatus Parser::CollectTypeQualifier(TypeCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_const:
    collector.GetTypeQualifierCollector().AddConst(ConsumeToken());
    break;
  case tok::kw_restrict:
    collector.GetTypeQualifierCollector().AddRestrict(ConsumeToken());
    break;
  case tok::kw_volatile:
    collector.GetTypeQualifierCollector().AddVolatile(ConsumeToken());
    break;
  case tok::kw_mutable:
    collector.GetTypeQualifierCollector().AddMutable(ConsumeToken());
    break;
  case tok::kw_pure:
    collector.GetTypeQualifierCollector().AddPure(ConsumeToken());
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}

bool Parser::IsTypeThunk(const Token &tk) {
  switch (tk.GetKind()) {
  case tok::star:
  case tok::amp:
    return true;
  default:
    return false;
  }
}

ASTStatus Parser::CollectTypeThunk(TypeCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::star:
    collector.GetTypeThunkCollector().AddPointer(ConsumeToken());
    break;
  case tok::amp:
    collector.GetTypeThunkCollector().AddReference(ConsumeToken());
    break;
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}
ASTStatus Parser::CollectTypeThunks(TypeCollector &collector) {

  assert(collector.GetTypeSpecifierCollector().HasAny() &&
         "Attemping to collect type-patterns without a type");

  if (!GetTok().IsTypeThunk() && GetTok().IsIdentifierOrUnderscore()) {
    collector.GetTypeThunkCollector().AddValue();
    return syn::MakeASTSuccess();
  }
  // TODO: Simple for now but this will be greatly expanded
  ASTStatus status;
  while (GetTok().IsTypeThunk()) {
    status = CollectTypeThunk(collector);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
ASTStatus Parser::CollectBasicTypeDecl(TypeCollector &collector) {

  if (!GetTok().IsBasicType()) {
    return syn::MakeASTCodeCompletionStatus();
  }
  switch (GetTok().GetKind()) {
  // TODO: Think about void here
  case tok::kw_void:
    collector.GetTypeSpecifierCollector().AddVoid(ConsumeToken());
    break;
  case tok::kw_auto:
    collector.GetTypeSpecifierCollector().AddAuto(ConsumeToken());
    break;
  case tok::kw_char:
    collector.GetTypeSpecifierCollector().AddChar(ConsumeToken());
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
  case tok::kw_imaginary32:
    collector.GetTypeSpecifierCollector().AddImaginary32(ConsumeToken());
  case tok::kw_imaginary64:
    break;
    collector.GetTypeSpecifierCollector().AddImaginary64(ConsumeToken());
    break;
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}
ASTStatus Parser::CollectNominalTypeDecl(TypeCollector &collector) {
  switch (GetTok().GetKind()) {
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
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}
ASTStatus Parser::CollectStorageSpecifier(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_static:
    collector.GetStorageSpecifierCollector().AddStatic(ConsumeToken());
    break;
  case tok::kw_register:
    collector.GetStorageSpecifierCollector().AddRegister(ConsumeToken());
    break;
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}
ASTStatus Parser::CollectFunctionDecl(ParsingDeclCollector &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_fun:
    collector.GetFunctionSpecifierCollector().AddFun(ConsumeToken());
    break;
  case tok::kw_inline:
    collector.GetFunctionSpecifierCollector().AddInline(ConsumeToken());
    break;
  default:
    return syn::MakeASTCodeCompletionStatus();
  }
  return syn::MakeASTSuccess();
}

ASTStatus Parser::VerifyDeclCollected(ParsingDeclCollector &collector) {
  return syn::MakeASTSuccess();
}
