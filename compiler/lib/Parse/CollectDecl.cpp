#include "stone/Basic/Defer.h"
#include "stone/Diag/ASTDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTNode.h"
#include "stone/Syntax/Stmt.h"

using namespace stone;

SyntaxStatus Parser::CollectDeclSpecifier(ParsingDecl &collector) {

  SyntaxStatus status;
  status = CollectImportSpecifier(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectAccessSpecifier(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectFunctionSpecifier(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectBasicTypeSpecifier(collector);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectNominalTypeSpecifier(collector);
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

SyntaxStatus Parser::CollectImportSpecifier(ParsingDecl &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_import:
    collector.GetImportSpecifierCollector().AddImport(ConsumeToken());
    break;
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectTypeOperator(ParsingDecl &collector) {
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
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectAccessSpecifier(ParsingDecl &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_public: {
    if (collector.GetAccessSpecifierCollector().HasPublic()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetAccessSpecifierCollector().AddPublic(ConsumeToken());
    break;
  }
  case tok::kw_internal: {
    if (collector.GetAccessSpecifierCollector().HasInternal()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetAccessSpecifierCollector().AddInternal(ConsumeToken());
    break;
  }
  case tok::kw_private:
    if (collector.GetAccessSpecifierCollector().HasPrivate()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetAccessSpecifierCollector().AddPrivate(ConsumeToken());
    break;
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
}

// TODO: Dulicate check
SyntaxStatus Parser::CollectTypeQualifiers(ParsingDecl &collector) {
  SyntaxStatus status;
  while (GetTok().IsQualifier()) {
    status = CollectTypeQualifier(collector);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
// TODO: Dulicate check
SyntaxStatus Parser::CollectTypeQualifier(ParsingDecl &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_const: {
    if (collector.GetTypeQualifierCollector().HasConst()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetTypeQualifierCollector().AddConst(ConsumeToken());
    break;
  }
  case tok::kw_mutable: {
    if (collector.GetTypeQualifierCollector().HasMutable()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetTypeQualifierCollector().AddMutable(ConsumeToken());
    break;
  }
  case tok::kw_immutable: {
    if (collector.GetTypeQualifierCollector().HasImmutable()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetTypeQualifierCollector().AddImmutable(ConsumeToken());
    break;
  }
  case tok::kw_pure: {
    if (collector.GetTypeQualifierCollector().HasPure()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetTypeQualifierCollector().AddPure(ConsumeToken());
    break;
  }
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
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

SyntaxStatus Parser::CollectTypeThunk(ParsingDecl &collector) {
  switch (GetTok().GetKind()) {
  case tok::star:
    collector.GetTypeThunkCollector().AddPointer(ConsumeToken());
    break;
  case tok::amp:
    collector.GetTypeThunkCollector().AddReference(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectTypeThunks(ParsingDecl &collector) {

  assert(collector.GetTypeSpecifierCollector().HasAny() &&
         "Attemping to collect type-patterns without a type");

  if (!GetTok().IsTypeThunk() && GetTok().IsIdentifierOrUnderscore()) {
    collector.GetTypeThunkCollector().AddValue();
    return MakeSyntaxSuccess();
  }
  // TODO: Simple for now but this will be greatly expanded
  SyntaxStatus status;
  while (GetTok().IsTypeThunk()) {
    status = CollectTypeThunk(collector);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
SyntaxStatus Parser::CollectBasicTypeSpecifier(ParsingDecl &collector) {

  if (!GetTok().IsBasicType()) {
    return MakeSyntaxCodeCompletionStatus();
  }
  switch (GetTok().GetKind()) {
  case tok::kw_any:
    collector.GetTypeSpecifierCollector().AddAny(ConsumeToken());
    break;
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
    break;
  case tok::kw_imaginary64:
    collector.GetTypeSpecifierCollector().AddImaginary64(ConsumeToken());
    break;
  case tok::kw_string:
    collector.GetTypeSpecifierCollector().AddString(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectNominalTypeSpecifier(ParsingDecl &collector) {
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
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectStorageSpecifier(ParsingDecl &collector) {
  switch (GetTok().GetKind()) {
  case tok::kw_static:
    collector.GetStorageSpecifierCollector().AddStatic(ConsumeToken());
    break;
  case tok::kw_register:
    collector.GetStorageSpecifierCollector().AddRegister(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectFunctionSpecifier(ParsingDecl &collector) {

  switch (GetTok().GetKind()) {
  case tok::kw_fun: {
    if (collector.GetFunctionSpecifierCollector().HasFun()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetFunctionSpecifierCollector().AddFun(ConsumeToken());
    break;
  }
  case tok::kw_inline: {
    if (collector.GetFunctionSpecifierCollector().HasInline()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    collector.GetFunctionSpecifierCollector().AddInline(ConsumeToken());
    break;
  }
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
