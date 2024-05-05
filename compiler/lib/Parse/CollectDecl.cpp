#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTNode.h"
#include "stone/AST/Stmt.h"
#include "stone/Basic/Defer.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"

using namespace stone;

ParserStatus Parser::CollectDeclSpecifier(ParsingDecl &parsingDecl) {

  ParserStatus status;
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

ParserStatus Parser::CollectImportSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_import:
    parsingDecl.GetImportSpecifierCollector().AddImport(ConsumeToken());
    break;
  default:
    return stone::MakeParserCodeCompletionStatus();
  }
  return stone::MakeParserSuccess();
}

ParserStatus Parser::CollectTypeOperator(ParsingDecl &parsingDecl) {
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
    return stone::MakeParserCodeCompletionStatus();
  }
  return stone::MakeParserSuccess();
}

ParserStatus Parser::CollectAccessSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_public: {
    if (parsingDecl.GetAccessSpecifierCollector().HasPublic()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.SetActiveDeclSpecKind(DeclSpecifierKind::Public);
    parsingDecl.GetAccessSpecifierCollector().AddPublic(ConsumeToken());
    break;
  }
  case tok::kw_internal: {
    if (parsingDecl.GetAccessSpecifierCollector().HasInternal()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.SetActiveDeclSpecKind(DeclSpecifierKind::Internal);
    parsingDecl.GetAccessSpecifierCollector().AddInternal(ConsumeToken());
    break;
  }
  case tok::kw_private:
    if (parsingDecl.GetAccessSpecifierCollector().HasPrivate()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.SetActiveDeclSpecKind(DeclSpecifierKind::Private);
    parsingDecl.GetAccessSpecifierCollector().AddPrivate(ConsumeToken());
    break;
  default:
    return stone::MakeParserCodeCompletionStatus();
  }
  return stone::MakeParserSuccess();
}

// TODO: Dulicate check
ParserStatus Parser::CollectTypeQualifiers(ParsingDecl &parsingDecl) {
  ParserStatus status;
  while (GetTok().IsQualifier()) {
    status = CollectTypeQualifier(parsingDecl);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
// TODO: Dulicate check
ParserStatus Parser::CollectTypeQualifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_const: {
    if (parsingDecl.GetTypeQualifierCollector().HasConst()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.SetActiveDeclSpecKind(DeclSpecifierKind::Const);
    parsingDecl.GetTypeQualifierCollector().AddConst(ConsumeToken());
    break;
  }
  case tok::kw_mutable: {
    if (parsingDecl.GetTypeQualifierCollector().HasMutable()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddMutable(ConsumeToken());
    break;
  }
  case tok::kw_final: {
    if (parsingDecl.GetTypeQualifierCollector().HasFinal()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddFinal(ConsumeToken());
    break;
  }
  case tok::kw_pure: {
    if (parsingDecl.GetTypeQualifierCollector().HasPure()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.GetTypeQualifierCollector().AddPure(ConsumeToken());
    break;
  }
  default:
    return stone::MakeParserCodeCompletionStatus();
  }
  return stone::MakeParserSuccess();
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

ParserStatus Parser::CollectTypeChunk(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::star:
    parsingDecl.GetTypeChunkCollector().AddPointer(ConsumeToken());
    break;
  case tok::amp:
    parsingDecl.GetTypeChunkCollector().AddReference(ConsumeToken());
    break;
  default:
    return MakeParserCodeCompletionStatus();
  }
  return MakeParserSuccess();
}
ParserStatus Parser::CollectTypeChunks(ParsingDecl &parsingDecl) {

  assert(parsingDecl.GetTypeSpecifierCollector().HasAny() &&
         "Attemping to collect type-patterns without a type");

  if (!GetTok().IsTypeChunk() && GetTok().IsIdentifierOrUnderscore()) {
    parsingDecl.GetTypeChunkCollector().AddValue();
    return MakeParserSuccess();
  }
  // TODO: Simple for now but this will be greatly expanded
  ParserStatus status;
  while (GetTok().IsTypeChunk()) {
    status = CollectTypeChunk(parsingDecl);
    if (status.HasCodeCompletion()) {
      return status;
    }
  }
  return status;
}
ParserStatus Parser::CollectBasicTypeSpecifier(ParsingDecl &parsingDecl) {

  if (!GetTok().IsBasicType()) {
    return MakeParserCodeCompletionStatus();
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
    return MakeParserCodeCompletionStatus();
  }
  return MakeParserSuccess();
}
ParserStatus Parser::CollectNominalTypeSpecifier(ParsingDecl &parsingDecl) {
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
    return MakeParserCodeCompletionStatus();
  }
  return MakeParserSuccess();
}
ParserStatus Parser::CollectStorageSpecifier(ParsingDecl &parsingDecl) {
  switch (GetTok().GetKind()) {
  case tok::kw_static:
    parsingDecl.GetStorageSpecifierCollector().AddStatic(ConsumeToken());
    break;
  case tok::kw_register:
    parsingDecl.GetStorageSpecifierCollector().AddRegister(ConsumeToken());
    break;
  default:
    return MakeParserCodeCompletionStatus();
  }
  return MakeParserSuccess();
}
ParserStatus Parser::CollectFunctionSpecifier(ParsingDecl &parsingDecl) {

  switch (GetTok().GetKind()) {
  case tok::kw_fun: {
    if (parsingDecl.GetFunctionSpecifierCollector().HasFun()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.GetFunctionSpecifierCollector().AddFun(ConsumeToken());
    break;
  }
  case tok::kw_inline: {
    if (parsingDecl.GetFunctionSpecifierCollector().HasInline()) {
      return stone::MakeParserCodeCompletionStatus();
    }
    parsingDecl.GetFunctionSpecifierCollector().AddInline(ConsumeToken());
    break;
  }
  default:
    return MakeParserCodeCompletionStatus();
  }
  return MakeParserSuccess();
}
