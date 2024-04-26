#include "stone/Basic/Defer.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/ASTDiagnostic.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTNode.h"
#include "stone/Syntax/Stmt.h"

using namespace stone;

SyntaxStatus Parser::CollectDeclSpecifier(ParsingDecl &parsingDecl) {

  SyntaxStatus status;
  status = CollectImportSpecifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectAccessSpecifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectFunctionSpecifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectBasicTypeSpecifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectNominalTypeSpecifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectTypeQualifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  status = CollectStorageSpecifier(parsingDecl);
  if (status.IsSuccess()) {
    return status;
  }
  // If we are here, we did not find anything
  status.SetHasCodeCompletion();
  return status;
}

SyntaxStatus Parser::CollectImportSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_import:
    parsingDecl.GetImportSpecifierCollector().AddImport(ConsumeToken());
    break;
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectTypeOperator(ParsingDecl &parsingDecl) {
  // if(parsingDecl.GetTypeOperatorCollector().HasAny()){
  // }
  switch (GetTok().GetKind()) {
  case tok::kw_new:
    parsingDecl.GetTypeOperatorCollector().AddNew(ConsumeToken());
    break;
  case tok::kw_delete:
    parsingDecl.GetTypeOperatorCollector().AddDelete(ConsumeToken());
    break;
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
}

SyntaxStatus Parser::CollectAccessSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_public: {
    if (parsingDecl.GetAccessSpecifierCollector().HasPublic()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetAccessSpecifierCollector().AddPublic(ConsumeToken());
    break;
  }
  case tok::kw_internal: {
    if (parsingDecl.GetAccessSpecifierCollector().HasInternal()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetAccessSpecifierCollector().AddInternal(ConsumeToken());
    break;
  }
  case tok::kw_private:
    if (parsingDecl.GetAccessSpecifierCollector().HasPrivate()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetAccessSpecifierCollector().AddPrivate(ConsumeToken());
    break;
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
}

// TODO: Dulicate check
SyntaxStatus Parser::CollectTypeQualifiers(ParsingDecl &parsingDecl) {
  SyntaxStatus status;
  while (GetTok().IsQualifier()) {
    status = CollectTypeQualifier(parsingDecl);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
// TODO: Dulicate check
SyntaxStatus Parser::CollectTypeQualifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_const: {
    if (parsingDecl.GetTypeQualifierCollector().HasConst()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddConst(ConsumeToken());
    break;
  }
  case tok::kw_mutable: {
    if (parsingDecl.GetTypeQualifierCollector().HasMutable()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddMutable(ConsumeToken());
    break;
  }
  case tok::kw_final: {
    if (parsingDecl.GetTypeQualifierCollector().HasFinal()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddFinal(ConsumeToken());
    break;
  }
  case tok::kw_pure: {
    if (parsingDecl.GetTypeQualifierCollector().HasPure()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddPure(ConsumeToken());
    break;
  }
  default:
    return stone::MakeSyntaxCodeCompletionStatus();
  }
  return stone::MakeSyntaxSuccess();
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

SyntaxStatus Parser::CollectTypeChunk(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::star:
    parsingDecl.GetTypeChunkCollector().AddPointer(ConsumeToken());
    break;
  case tok::amp:
    parsingDecl.GetTypeChunkCollector().AddReference(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectTypeChunks(ParsingDecl &parsingDecl) {

  assert(parsingDecl.GetTypeSpecifierCollector().HasAny() &&
         "Attemping to collect type-patterns without a type");

  if (!GetTok().IsTypeChunk() && GetTok().IsIdentifierOrUnderscore()) {
    parsingDecl.GetTypeChunkCollector().AddValue();
    return MakeSyntaxSuccess();
  }
  // TODO: Simple for now but this will be greatly expanded
  SyntaxStatus status;
  while (GetTok().IsTypeChunk()) {
    status = CollectTypeChunk(parsingDecl);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
SyntaxStatus Parser::CollectBasicTypeSpecifier(ParsingDecl &parsingDecl) {

  if (!GetTok().IsBasicType()) {
    return MakeSyntaxCodeCompletionStatus();
  }
  switch (GetTok().GetKind()) {
  case tok::kw_any:
    parsingDecl.GetTypeSpecifierCollector().AddAny(ConsumeToken());
    break;
  case tok::kw_void:
    parsingDecl.GetTypeSpecifierCollector().AddVoid(ConsumeToken());
    break;
  case tok::kw_auto:
    parsingDecl.GetTypeSpecifierCollector().AddAuto(ConsumeToken());
    break;
  case tok::kw_char:
    parsingDecl.GetTypeSpecifierCollector().AddChar(ConsumeToken());
    break;
  case tok::kw_int:
    parsingDecl.GetTypeSpecifierCollector().AddInt(ConsumeToken());
    break;
  case tok::kw_int8:
    parsingDecl.GetTypeSpecifierCollector().AddInt8(ConsumeToken());
    break;
  case tok::kw_int16:
    parsingDecl.GetTypeSpecifierCollector().AddInt16(ConsumeToken());
    break;
  case tok::kw_int32:
    parsingDecl.GetTypeSpecifierCollector().AddInt32(ConsumeToken());
    break;
  case tok::kw_int64:
    parsingDecl.GetTypeSpecifierCollector().AddInt64(ConsumeToken());
    break;
  case tok::kw_uint:
    parsingDecl.GetTypeSpecifierCollector().AddUInt(ConsumeToken());
    break;
  case tok::kw_uint8:
    parsingDecl.GetTypeSpecifierCollector().AddUInt8(ConsumeToken());
    break;
  case tok::kw_byte:
    parsingDecl.GetTypeSpecifierCollector().AddByte(ConsumeToken());
    break;
  case tok::kw_uint16:
    parsingDecl.GetTypeSpecifierCollector().AddUInt16(ConsumeToken());
    break;
  case tok::kw_uint32:
    parsingDecl.GetTypeSpecifierCollector().AddUInt32(ConsumeToken());
    break;
  case tok::kw_uint64:
    parsingDecl.GetTypeSpecifierCollector().AddUInt64(ConsumeToken());
    break;
  case tok::kw_float:
    parsingDecl.GetTypeSpecifierCollector().AddFloat(ConsumeToken());
    break;
  case tok::kw_float32:
    parsingDecl.GetTypeSpecifierCollector().AddFloat32(ConsumeToken());
    break;
  case tok::kw_float64:
    parsingDecl.GetTypeSpecifierCollector().AddFloat64(ConsumeToken());
    break;
  case tok::kw_complex32:
    parsingDecl.GetTypeSpecifierCollector().AddComplex32(ConsumeToken());
    break;
  case tok::kw_complex64:
    parsingDecl.GetTypeSpecifierCollector().AddComplex64(ConsumeToken());
    break;
  case tok::kw_imaginary32:
    parsingDecl.GetTypeSpecifierCollector().AddImaginary32(ConsumeToken());
    break;
  case tok::kw_imaginary64:
    parsingDecl.GetTypeSpecifierCollector().AddImaginary64(ConsumeToken());
    break;
  case tok::kw_string:
    parsingDecl.GetTypeSpecifierCollector().AddString(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectNominalTypeSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_enum:
    parsingDecl.GetTypeSpecifierCollector().AddEnum(ConsumeToken());
    break;
  case tok::kw_struct:
    parsingDecl.GetTypeSpecifierCollector().AddStruct(ConsumeToken());
    break;
  case tok::kw_interface:
    parsingDecl.GetTypeSpecifierCollector().AddInterface(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectStorageSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_static:
    parsingDecl.GetStorageSpecifierCollector().AddStatic(ConsumeToken());
    break;
  case tok::kw_register:
    parsingDecl.GetStorageSpecifierCollector().AddRegister(ConsumeToken());
    break;
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
SyntaxStatus Parser::CollectFunctionSpecifier(ParsingDecl &parsingDecl) {

  switch (GetTok().GetKind()) {
  case tok::kw_fun: {
    if (parsingDecl.GetFunctionSpecifierCollector().HasFun()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetFunctionSpecifierCollector().AddFun(ConsumeToken());
    break;
  }
  case tok::kw_inline: {
    if (parsingDecl.GetFunctionSpecifierCollector().HasInline()) {
      return stone::MakeSyntaxCodeCompletionStatus();
    }
    parsingDecl.GetFunctionSpecifierCollector().AddInline(ConsumeToken());
    break;
  }
  default:
    return MakeSyntaxCodeCompletionStatus();
  }
  return MakeSyntaxSuccess();
}
