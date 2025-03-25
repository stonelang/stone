#include "stone/AST/DeclState.h"
#include "stone/AST/DiagnosticsParse.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
using namespace stone;

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
    ParsingDeclState PDS(*this);
    PDS.GetParsingDeclOptions().AddAllowTopLevel();
    auto result = ParseTopLevelDecl(PDS);

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

ParserResult<Decl> Parser::ParseTopLevelDecl(ParsingDeclState &PDS) {

  // Make sure we have a top level-decl
  assert(IsTopLevelDeclParsing() &&
         "ParseTopLevelDecl requires a top-level decl");

  // assert((GetCurScope() == nullptr) && "A scope is already active?");

  // ParsingScope scope(*this, ASTScopeKind::TopLevelDecl,
  //                    "parsing top-level declaration");

  return ParseDecl(PDS);
}

ParserStatus Parser::ParseDeclModifiers(DeclPropertyList &modifiers) {
  ParserStatus status;
  while (IsParsing()) {
    switch (GetCurTok().GetKind()) {
    case tok::kw_public: {
      modifiers.AddPublic(ConsumeToken());
      continue;
    }
    case tok::kw_internal: {
      modifiers.AddInternal(ConsumeToken());
      continue;
    }
    case tok::kw_private: {
      modifiers.AddPrivate(ConsumeToken());
      continue;
    }
    case tok::kw_static: {
      modifiers.AddStatic(ConsumeToken());
      continue;
    }
    // case tok::kw_extern: {
    //   PDS.AddExternModifier(ConsumeToken());
    //   continue;
    // }
    default:
      break;
    }
    return status;
  }
}
ParserStatus Parser::ParseDeclAttributes(DeclPropertyList &attributes) {
  ParserStatus status;

  return status;
}
ParserResult<Decl> Parser::ParseDecl(ParsingDeclState &PDS) {

  ParserResult<Decl> declResult;
  auto status = ParseDeclAttributes(PDS.GetDeclPropertyList());

  if (status.IsError()) {
    return declResult;
  }
  status |= ParseDeclModifiers(PDS.GetDeclPropertyList());
  if (status.IsError()) {
    return declResult;
  }
  status |= ParseTypeAttributes(PDS.GetTypePropertyList());
  if (status.IsError()) {
    return declResult;
  }
  status |= ParseTypeModifiers(PDS.GetTypePropertyList());
  if (status.IsError()) {
    return declResult;
  }

  switch (GetCurTok().GetKind()) {
  case tok::kw_import: {
    declResult = ParseImportDecl(PDS);
    break;
  }
  case tok::kw_fun: {
    declResult = ParseFunDecl(PDS);
    break;
  }
  case tok::kw_struct: {
    declResult = ParseStructDecl(PDS);
    break;
  }
  default: {
    if (GetCurTok().IsBuiltin()) {
      declResult = ParseVarDecl(PDS);
      break;
    }
  }
  }
  return declResult;
}

ParserResult<ImportDecl> Parser::ParseImportDecl(ParsingDeclState &PDS) {
  assert(GetCurTok().IsImport() &&
         "ParseImportDecl requires a import specifier");

  return nullptr;
}

ParserResult<FunDecl> Parser::ParseFunDecl(ParsingDeclState &PDS) {
  assert(GetCurTok().IsFun() && "ParseFunDecl requires a fun specifier");

  auto typeStateResult = ParseType();
  assert(typeStateResult && "Expected a FunctionTypeState!");

  auto functionTypeState =
      static_cast<FunctionTypeState *>(typeStateResult.Get());

  PDS.GetDeclState()->SetTypeState(functionTypeState);
  ParserStatus status;

  // spec.SetParsingTypeSpec(funTypeSpec.Get());

  // ParserStatus status;
  // SrcLoc basicNameLoc;
  // status = ParseIdentifier(spec.basicName, basicNameLoc);
  // spec.basicNameLoc.SetLoc(basicNameLoc);

  // // Now, parse the function signature
  status |= ParseFunctionSignature(*functionTypeState);

  // if (status.IsError()) {
  //   return status;
  // }
  status |= ParseFunctionBody(*functionTypeState);

  // auto FD = FunDecl::Create(
  //     GetASTContext(), spec.GetParsingFunTypeSpec()->GetStatic(),
  //     spec.GetParsingFunTypeSpec()->GetLoc(), spec.declName,
  //     spec.declNameLoc.GetLoc(),
  //     spec.GetParsingFunTypeSpec()->GetResultType()->GetType(),
  //     GetCurDeclContext());

  // // Very simple for the time being
  // return stone::MakeParserResult<FunDecl>(FD);
}

ParserStatus Parser::ParseFunctionSignature(FunctionTypeState &FTS) {

  // assert(PDS.GetDeclState()->HasTypeState() &&
  //        "ParseFunctionSignature requires a FunctionTypeState");

  // assert(PDS.GetDeclState()->GetTypeState()->IsFunction() &&
  //        "ParseFunctionSignature TypeSate is not FunctionTypeState");

  // auto functionTypeState = PDS.GetDeclState()->GetTypeState();

  // ParserStatus status;

  // status = ParseFunctionArguments(spec);
  // if (status.IsError()) {
  //   return status;
  // }

  // spec.declName = DeclName(spec.basicName);

  // // if (!GetCurTok().IsArrow()) {
  // //   status.SetIsError();
  // //   return status;
  // // }

  // SrcLoc arrowLoc;
  // if (!ConsumeIf(tok::arrow, arrowLoc)) {
  //   // FixIt ':' to '->'.
  //   diagnose(GetCurTok(), diag::error_expected_arrow_after_function_decl)
  //       .fixItReplace(curTok.GetLoc(), llvm::StringRef("->"));
  //   // arrowLoc = ConsumeToken(tok::colon);
  //   return MakeParserError();
  // }

  // funTypeSpec->SetArrow(arrowLoc);

  // // Before we check for qualifiers, there should not be any because this
  // is
  // a
  // // function fun Print() -> const T {}
  // // TODO: Check for qualifiers
  // status |= ParseQualifierList(spec);
  // auto resultType =
  //     ParseDeclResultType(diag::error_expected_type_for_function_result);

  // // Update the decl-spec with the result type
  // funTypeSpec->SetResultType(resultType.Get());

  // Jsut return success for now
  return MakeParserSuccess();
}

// TODO: Pass in the function spec in the future.
ParserStatus Parser::ParseFunctionArguments(FunctionTypeState &FTS) {

  // auto parsingFunTypeSpec = spec.GetParsingFunTypeSpec();

  // if (!GetCurTok().IsLParen()) {
  //   return MakeParserError();
  // }
  // parsingFunTypeSpec->SetLParen(ConsumeToken(tok::l_paren));

  // if (!GetCurTok().IsRParen()) {
  //   return MakeParserError();
  // }
  // parsingFunTypeSpec->SetRParen(ConsumeToken(tok::r_paren));

  return MakeParserSuccess();
}

ParserResult<TypeState>
Parser::ParseFunctionResultType(FunctionTypeState &FTS) {

  if (!FTS.HasArrow()) {
    return MakeParserSuccess();
  }
  // Check for modifiers
  TypePropertyList modifiers(GetASTContext());
  ParseTypeModifiers(modifiers);

  // auto resultTypeState = ParseType();

  // functionTypeState->SetResultTypeState(resultTypeState);

  return MakeParserSuccess();
}

ParserStatus Parser::ParseFunctionBody(FunctionTypeState &FTS) {
  // assert(spec.HasParsingTypeSpec() &&
  //        "ParseFunctionSignature requires a type-spec");
  // assert(spec.GetParsingTypeSpec()->IsFunction() &&
  //        "ParseFunctionSignature type-pec is not function");

  // auto parsingFunTypeSpec = spec.GetParsingFunTypeSpec();

  // // TODO:  BraceStmtPair braceStmtPair;

  // // This is where you what to start a BracePairDelimeter
  // ParserStatus status;
  // // ParsingScope funBodyScope(*this, ScopeKind::FunctionBody,
  // //                           "parsing fun arguments");

  // assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  // auto lParenLoc = ConsumeToken(tok::l_brace);

  // assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  // auto rParenLoc = ConsumeToken(tok::r_brace);

  // // Simple for now
  // auto FB = BraceStmt::Create(lParenLoc, {}, rParenLoc, GetASTContext());

  // parsingFunTypeSpec->SetBody(FB);

  // // parsingFunTypeSpec->SetBody(functionBody,
  // // FunctionDecl::BodyStatus::Parsed);

  // return status;
}

ParserResult<VarDecl> Parser::ParseVarDecl(ParsingDeclState &PDS) {
  ParserResult<VarDecl> result;
  auto typeState = ParseType();

  return result;
}

// ParserStatus Parser::ParseFunctionArguments(DeclState* DS) {

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

ParserResult<StructDecl> Parser::ParseStructDecl(ParsingDeclState &PDS) {}
