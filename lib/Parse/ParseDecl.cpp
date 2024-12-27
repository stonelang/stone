#include "stone/AST/DiagnosticsParse.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/ParsingDeclSpec.h"

using namespace stone;

ParsingDeclSpec::~ParsingDeclSpec() {}

bool Parser::IsStartOfDecl() {
  return curTok.IsAny(tok::kw_public, tok::kw_internal, tok::kw_private,
                      tok::kw_fun, tok::kw_struct, tok::kw_class,
                      tok::kw_interface, tok::kw_enum, tok::kw_const,
                      tok::kw_import, tok::kw_space, tok::kw_static);
}

bool Parser::IsTopLevelDeclParsing() {
  return ((IsParsing() && IsStartOfDecl()) ? true : false);
}

void Parser::AddTopLevelDecl(ParserResult<Decl> result) {
  assert(result.IsNonNull() && "Null Decl");
  GetSourceFile().AddTopLevelDecl(result.Get());
}

bool Parser::ParseTopLevelDecls() {

  if (curTok.Is(tok::LAST)) {
    ConsumeToken();
  }
  /// Create top-level scope
  auto ParsedTopLevelDecl = [&](ParserResult<Decl> &result) -> bool {
    return (!result.IsError() && !HasError() && result.IsNonNull());
  };

  while (IsTopLevelDeclParsing()) {
    ParsingDeclSpec spec(*this);
    spec.GetParsingDeclOptions().AddAllowTopLevel();
    auto result = ParseTopLevelDecl(spec);
    if (!ParsedTopLevelDecl(result)) {
      return false;
    }
    if (HasCodeCompletionCallbacks()) {
      GetCodeCompletionCallbacks()->CompletedParseTopLevelDecl(result.Get());
    }
    AddTopLevelDecl(result);
  }
  return true;
}

ParserResult<Decl> Parser::ParseTopLevelDecl(ParsingDeclSpec &spec) {

  // Make sure we have a top level-decl
  assert(IsTopLevelDeclParsing() &&
         "ParseTopLevelDecl requires a top-level decl");

  // assert((GetCurScope() == nullptr) && "A scope is already active?");

  // ParsingScope scope(*this, ASTScopeKind::TopLevelDecl,
  //                    "parsing top-level declaration");

  return ParseDecl(spec);
}

ParserStatus Parser::ParseQualifierList(ParsingDeclSpec &spec) {
  ParserStatus status;
  while (IsParsing()) {
    switch (GetCurTok().GetKind()) {
    case tok::kw_const: {
      spec.GetConstQualifier().SetLoc(ConsumeToken());
      break;
    }
    case tok::kw_stone: {
      spec.GetStoneQualifier().SetLoc(ConsumeToken());
      break;
    }
    case tok::kw_restrict: {
      spec.GetRestrictQualifier().SetLoc(ConsumeToken());
      break;
    }
    case tok::kw_volatile: {
      spec.GetVolatileQualifier().SetLoc(ConsumeToken());
      break;
    }
    default:
      break;
    }
    return status;
  }
}

ParserStatus Parser::ParsePublicVisibilityList(ParsingDeclSpec &spec) {
  ParserStatus status;
  while (IsParsing()) {
    switch (GetCurTok().GetKind()) {
    case tok::kw_public: {
      spec.GetPublicVisibility().SetLoc(ConsumeToken());
      continue;
    }
    case tok::kw_internal: {
      spec.GetInternalVisibility().SetLoc(ConsumeToken());
      continue;
    }
    case tok::kw_private: {
      spec.GetPrivateVisibility().SetLoc(ConsumeToken());
      continue;
    }
    default:
      break;
    }
    return status;
  }
}

ParserResult<Decl> Parser::ParseDecl(ParsingDeclSpec &spec) {

  ParserResult<Decl> DeclResult;
  auto status = ParseQualifierList(spec);
  if (status.IsError() || spec.HasQualifierOverflow()) {
    return DeclResult;
  }
  status |= ParsePublicVisibilityList(spec);
  if (status.IsError() || spec.HasVisibilityOverflow()) {
    return DeclResult;
  }

  switch (GetCurTok().GetKind()) {
  case tok::kw_import: {
    DeclResult = ParseImportDecl(spec);
    break;
  }
  case tok::kw_void:
  case tok::kw_auto:
  case tok::kw_char:
  case tok::kw_char8:
  case tok::kw_char16:
  case tok::kw_char32:
  case tok::kw_int:
  case tok::kw_int8:
  case tok::kw_int16:
  case tok::kw_int32:
  case tok::kw_int64:
  case tok::kw_uint:
  case tok::kw_uint8:
  case tok::kw_uint16:
  case tok::kw_uint32:
  case tok::kw_uint64:
  case tok::kw_float:
  case tok::kw_float32:
  case tok::kw_float64:
  case tok::kw_complex32:
  case tok::kw_complex64:
  case tok::kw_imaginary32:
  case tok::kw_imaginary64: {
    DeclResult = ParseVarDecl(spec);
    break;
  }
  case tok::kw_fun: {
    DeclResult = ParseFunDecl(spec);
    break;
  }
  case tok::kw_struct: {
    DeclResult = ParseStructDecl(spec);
    break;
  }
  }
  return DeclResult;
}

ParserResult<ImportDecl> Parser::ParseImportDecl(ParsingDeclSpec &spec) {
  assert(GetCurTok().IsImport() &&
         "ParseImportDecl requires a import specifier");

  return nullptr;
}

ParserResult<FunDecl> Parser::ParseFunDecl(ParsingDeclSpec &spec) {

  assert(GetCurTok().IsFun() && "ParseFunDecl requires a fun specifier");

  auto parsingTypeSpecResult = ParseType();
  assert(parsingTypeSpecResult && "Expected a fun type!");

  spec.SetParsingTypeSpec(parsingTypeSpecResult.Get());

  ParserStatus status;
  SrcLoc basicNameLoc;
  status = ParseIdentifier(spec.basicName, basicNameLoc);
  spec.basicNameLoc.SetLoc(basicNameLoc);

  // Now, parse the function signature
  status |= ParseFunctionSignature(spec);

  if (status.IsError()) {
    return status;
  }
}

ParserStatus Parser::ParseFunctionSignature(ParsingDeclSpec &spec) {

  assert(spec.HasParsingTypeSpec() &&
         "ParseFunctionSignature requires a type-spec");
  assert(spec.GetParsingTypeSpec()->IsFunction() &&
         "ParseFunctionSignature type-pec is not function");

  auto parsingFunTypeSpec = spec.GetParsingFunTypeSpec();

  ParserStatus status;

  status = ParseFunctionArguments(spec);
  if (status.IsError()) {
    return status;
  }

  spec.declName = DeclName(spec.basicName);

  // if (!GetCurTok().IsArrow()) {
  //   status.SetIsError();
  //   return status;
  // }

  SrcLoc arrowLoc;
  if (!ConsumeIf(tok::arrow, arrowLoc)) {
    // FixIt ':' to '->'.
    diagnose(GetCurTok(), diag::error_expected_arrow_after_function_decl)
        .fixItReplace(curTok.GetLoc(), llvm::StringRef("->"));
    // arrowLoc = ConsumeToken(tok::colon);
    return MakeParserError();
  }

  parsingFunTypeSpec->SetArrow(arrowLoc);

  // auto retType = ParseType(spec,
  //     diag::error_expected_type_for_function_result);

  return MakeParserSuccess();
}

// TODO: Pass in the function spec in the future.
ParserStatus Parser::ParseFunctionArguments(ParsingDeclSpec &spec) {

  assert(spec.HasParsingTypeSpec() &&
         "ParseFunctionSignature requires a type-spec");
  assert(spec.GetParsingTypeSpec()->IsFunction() &&
         "ParseFunctionSignature type-pec is not function");

  auto parsingFunTypeSpec = spec.GetParsingFunTypeSpec();

  if (!GetCurTok().IsLParen()) {
    return MakeParserError();
  }
  parsingFunTypeSpec->SetLParen(ConsumeToken(tok::l_paren));

  if (!GetCurTok().IsRParen()) {
    return MakeParserError();
  }
  parsingFunTypeSpec->SetRParen(ConsumeToken(tok::r_paren));

  return MakeParserSuccess();
}

ParserResult<VarDecl> Parser::ParseVarDecl(ParsingDeclSpec &spec) {
  ParserResult<VarDecl> result;
  auto parsingTypeSpecResult = ParseType();

  return result;
}

// ParserStatus Parser::ParseFunctionSignature(ParsingDeclSpec &spec,
//                                             Identifier basicName,
//                                             DeclName &fullName) {

//   ParserStatus status;
//   status |= ParseFunctionArguments(spec);

//   auto retType = ParseType(spec,
//   diag::error_expected_type_for_function_result);
// }

// ParserStatus Parser::ParseFunctionArguments(ParsingDeclSpec &spec) {

//   ParserStatus status;
//   SrcLoc lParenLoc;
//   SrcLoc rParenLoc;
// }

// /// Return a parsed decl name
// ParsedDeclName Parser::ParseDeclName(llvm::StringRef name) const {
//   return ParsedDeclName();
// }

// /// Form a Swift declaration name from its constituent parts.
// DeclName Parser::FormDeclName(ASTContext &ctx, StringRef baseName,
//                               ArrayRef<StringRef> argumentLabels,
//                               bool isFunctionName, bool isInitializer,
//                               bool isSubscript) {

//   return DeclName();
// }

ParserResult<StructDecl> Parser::ParseStructDecl(ParsingDeclSpec &spec) {}
