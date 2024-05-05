#include "stone/AST/Stmt.h"
#include "stone/Basic/Defer.h"
#include "stone/IDE.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
// #include "stone/AST/Using.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/ASTNode.h"
#include "stone/AST/Stmt.h"

using namespace stone;

void Parser::ParseTopLevelDecls(
    llvm::SmallVector<ParserResult<Decl>> &results) {
  // Prime the Parser's curTok
  // The Lexer has the first curTok but the Parser's curTok defaults to tk::MAX
  // So, update the parser's curTok with the first curTok from the Lexer
  if (curTok.Is(tok::MAX)) {
    ConsumeToken();
  }
  auto Success = [&](ParserResult<Decl> &result) -> bool {
    return (!result.IsError() && !HasError() && result.IsNonNull());
  };
  while (IsParsing() && IsTopLevelDeclSpecifier()) {
    ParsingDecl parsingDecl(*this);
    parsingDecl.parsingDeclOpts = ParsingDeclFlags::AllowTopLevel;
    auto result = ParseTopLevelDecl(parsingDecl);
    if (!Success(result)) {
      return;
    }
    if (HasCodeCompletionCallbacks()) {
      GetCodeCompletionCallbacks()->CompletedParseTopLevelDecl(result.Get());
    }
    results.push_back(result);
  }
}
// Ex: sample.stone
// fun F0() -> void {}
// fun F1() -> void {}
// There are two top decls - F0 and F1
// This call parses one at a time and adds it to the SourceFile
ParserResult<Decl> Parser::ParseTopLevelDecl(ParsingDecl &parsingDecl) {

  assert(GetCurScope() == nullptr && "A scope is already active?");
  ParsingScope parseTopLevelDeclScope(*this, ScopeKind::TopLevel,
                                      "parsing top-level declaration");
  ParserResult<Decl> result;
  while (result.IsNull() && IsParsing() && IsTopLevelDeclSpecifier()) {
    if (CollectDeclSpecifier(parsingDecl).HasCodeCompletion()) {
      // This is an empty file -- stop parsing.
      return result;
    }
    if (parsingDecl.IsAccessLevelActive() || parsingDecl.IsTypeQualsActive()) {
      continue;
    }
    result = ParseDecl(parsingDecl);
  }
  return result;
}

/// Parse declaration specs
ParserResult<Decl> Parser::ParseDecl(ParsingDecl &parsingDecl) {

  ParsingScope parseDeclScope(*this, ScopeKind::Decl, "parsing declaration");

  if (parsingDecl.GetImportSpecifierCollector().HasImport()) {
    return ParseImportDecl(parsingDecl);
  } else if (parsingDecl.GetFunctionSpecifierCollector().HasFun()) {
    return ParseFunDecl(parsingDecl);
  } else if (parsingDecl.GetTypeSpecifierCollector().IsStruct()) {
    return ParseStructDecl(parsingDecl);
  } else if (parsingDecl.GetTypeSpecifierCollector().IsEnum()) {
    return ParseEnumDecl(parsingDecl);
  } else if (parsingDecl.GetTypeSpecifierCollector().IsInterface()) {
    return ParseInterfaceDecl(parsingDecl);
  } else if (parsingDecl.GetTypeSpecifierCollector().IsAuto()) {
    return ParseAutoDecl(parsingDecl);
  }
  return ParserResult<Decl>();
}
bool Parser::IsTopLevelDeclSpecifier() { return GetTok().IsTopLevel(); }

void Parser::ParseDeclName() {}

ParserResult<Decl> Parser::ParseVarDecl(ParsingDecl &parsingDecl) {

  ParserResult<Decl> result;
  ParsingScope varDeclScope(*this, ScopeKind::VarDecl,
                            "parsing var declaration");

  assert(parsingDecl.GetTypeSpecifierCollector().HasAny() &&
         "Attempting to parse type-patterns without a type specified");

  // TODO: Significant improvement required here. This is a starter.
  // May just require the TypeQualifierCollector to add 'final' on creation but
  // remove it if mutable is added

  if (!parsingDecl.GetTypeQualifierCollector().HasAny()) {
    parsingDecl.GetTypeQualifierCollector().AddFinal(SrcLoc());
  }

  CollectTypeChunks(parsingDecl);

  assert(parsingDecl.GetTypeChunkCollector().HasAny() &&
         "Type is missing a type-thunk");

  auto varDecl = VarDecl::Create(GetASTContext());

  return result;
}

ParserResult<Decl> Parser::ParseAutoDecl(ParsingDecl &parsingDecl) {

  ParserResult<Decl> result;
  ParsingScope autoDeclScope(*this, ScopeKind::AutoDecl,
                             "parsing auto storage declaration");

  // ParsingDeclaratorCollector declaratorCollector(parsingDecl,
  // DeclaratorScopeKind::Variable); auto status =
  // ParseDeclarator(declaratorCollector);

  return result;
}

ParserResult<Decl> Parser::ParseFunDecl(ParsingDecl &parsingDecl) {

  ParsingScope funDeclScope(*this, ScopeKind::FunDecl,
                            "parsing fun declaration");

  assert(parsingDecl.GetFunctionSpecifierCollector().HasFun() &&
         "Attempting to parse a function without a function definition.");

  assert(parsingDecl.GetFunctionSpecifierCollector().GetFunLoc().isValid() &&
         "Attempting to parse a function without a valid 'fun' locaiton.");

  // TODO: Here or in ParseFunctionType
  parsingDecl.GetTypeChunkCollector().AddFunction();

  // At this stage, only the pure modifier is allowed
  if (parsingDecl.GetTypeQualifierCollector().HasAny() &&
      !parsingDecl.GetTypeQualifierCollector().IsPure()) {
    // Do some logging
    return MakeParserError();
  }

  if (parsingDecl.GetTypeSpecifierCollector().HasAny()) {
    // TODO: Log a message -- not allowed to have type specs here
    return MakeParserError();
  }

  // Make sure we have a valid identifier
  if (!GetTok().IsIdentifierOrUnderscore()) {
    // Do some logging  "Expecting function declarator or identifier");
    return MakeParserError();
  }

  ParserStatus status;
  Identifier basicName;
  SrcLoc nameLoc;
  status = ParseIdentifier(basicName, nameLoc);

  // TODO: May want to move to ParseFunctionSignaure
  Identifier parentName;
  SrcLoc parentNameLoc;

  // TODO: You have to perform a name look-up where because you will be
  // dealing with "identifier::""
  if (GetTok().IsDoubleColon()) {
    if (parsingDecl.GetStorageSpecifierCollector().HasStatic()) {
      // TODO: Log
      return MakeParserError();
    }
    // TODO: You are consuming the double colon
    parsingDecl.GetFunctionSpecifierCollector().AddIsMember(ConsumeToken());

    if (!GetTok().IsIdentifierOrUnderscore()) {
      // Do some logging  "Expecting Parent identifier");
      return MakeParserError();
    }
    // TODO: That identifier should already exist
    // status = ParseIdentifier(parentName, parentNameLoc);
  }

  if (parsingDecl.GetStorageSpecifierCollector().HasStatic() &&
      parsingDecl.GetFunctionSpecifierCollector().HasIsMember()) {
    // Log only member functions can be status
    return MakeParserError();
  }

  DeclName fullName;
  // Now, parse the function signature
  status |= ParseFunctionSignature(parsingDecl, basicName, fullName);

  if (status.IsError()) {
    return status;
  }

  parsingDecl.GetDeclNameCollector().SetName(fullName);
  parsingDecl.GetDeclNameCollector().SetLoc(nameLoc);

  // TODO: Ok for now.
  if (parsingDecl.GetTypeSpecifierCollector().GetType().IsNull()) {
    status.SetIsError();
    return status;
  }

  // Apply what what collected
  // parsingDecl.Apply();
  // Create the function
  auto funDecl = FunDecl::Create(parsingDecl, astContext, GetCurDeclContext());
  assert(funDecl);

  // TODO: Find a better place for this -- maybe pass as parameter
  if (parsingDecl.parsingDeclOpts.contains(ParsingDeclFlags::AllowTopLevel)) {
    funDecl->SetIsTopLevel();
  }

  if (parsingDecl.GetFunctionSpecifierCollector().HasIsMember()) {
  }

  if (!status.HasCodeCompletion()) {
    status |= ParseFunctionBody(parsingDecl, *funDecl);
  }
  // Very simple for the time being
  return stone::MakeParserResult<Decl>(funDecl);
}

ParserStatus Parser::ParseFunctionSignature(ParsingDecl &parsingDecl,
                                            Identifier basicName,
                                            DeclName &fullName) {
  ParserStatus status;
  ParsingScope funSigScope(*this, ScopeKind::FunctionSignature,
                           "parsing fun signature");

  status |= ParseFunctionArguments(parsingDecl);
  if (status.IsError()) {
    return status;
  }

  // TODO: simple for now
  fullName = DeclName(basicName);

  SrcLoc arrowLoc;
  if (GetTok().IsArrow()) {
    ParsingScope functionResult(*this, ScopeKind::ReturnClause,
                                "parsing result");

    if (!ConsumeIf(tok::arrow, arrowLoc)) {
      // FixIt ':' to '->'.
      diagnose(curTok, diag::error_expected_arrow_after_function_param)
          .fixItReplace(curTok.GetLoc(), llvm::StringRef("->"));
      // arrowLoc = ConsumeToken(tok::colon);
      // return MakeParserError();
    } else {
      parsingDecl.GetFunctionSpecifierCollector().AddArrowLoc(arrowLoc);
    }

    if (parsingDecl.GetTypeQualifierCollector().HasAny()) {
      // if (!parsingDecl.GetTypeQualifierCollector().HasPureOnly()) {
      //   // TODO: Log
      //   status.SetHasCodeCompletion();
      //   return status;
      // }

      status.SetHasCodeCompletion();
      return status;
    }
    // We can call collect here to get the return type
    // parsingDecl.CollectUntil(tok::l_brace);
    // if (!parsingDecl.GetTypeSpecifierCollector().HasAny()) {
    //   // Perform some logging function must return a function type
    //   status.SetIsError();
    //   return status;
    // }
    // TODO: Look for TypeSpecs

    // Collect Qualifiers
    CollectTypeQualifiers(parsingDecl);

    // Why not just ParseFunctionType
    auto retType = ParseDeclResultType(
        parsingDecl, diag::error_expected_type_for_function_result);

    parsingDecl.GetTypeSpecifierCollector().SetType(retType);

    // status |= ParseFunctionResult(parsingDecl);
    // assert(curTok.Is(tok::arrow) && "Require '->'");
    // auto arrowLoc = ConsumeToken(tok::arrow);

    // ParseReturnType();

    // Parse the return type
    // funDecl->SetReturnType();

    // ConsumeToken();
    return status;
  }
}
ParserStatus Parser::ParseFunctionArguments(ParsingDecl &parsingDecl) {

  SrcLoc lParenLoc;
  SrcLoc rParenLoc;

  // ParenTracker
  ParsingScope funArgScope(*this, ScopeKind::FunctionArguments,
                           "parsing fun arguments");

  // ParenPair
  /// TODO: Handle
  /// Name(int i)
  /// Name(int i, () -> void)
  if (GetTok().IsLParen()) {
    lParenLoc = ConsumeToken(tok::l_paren);
  } else {
    // If we don't have the leading '(', complain.
    // auto diag = diagnose(Tok, diagID);
    return MakeParserError();
  }

  if (GetTok().IsRParen()) {
    lParenLoc = ConsumeToken(tok::r_paren);
  } else {
    // If we don't have the leading '(', complain.
    // auto diag = diagnose(Tok, diagID);
    return MakeParserError();
  }
  return MakeParserSuccess();
}

ParserStatus Parser::ParseFunctionBody(ParsingDecl &parsingDecl,
                                       FunctionDecl &funDecl) {

  // TODO:  BraceStmtPair braceStmtPair;

  // This is where you what to start a BracePairDelimeter
  ParserStatus status;
  ParsingScope funBodyScope(*this, ScopeKind::FunctionBody,
                            "parsing fun arguments");

  assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  auto lParenLoc = ConsumeToken(tok::l_brace);

  assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  auto rParenLoc = ConsumeToken(tok::r_brace);

  // Simple for now
  auto functionBody =
      BraceStmt::Create(lParenLoc, {}, rParenLoc, GetASTContext());

  funDecl.SetBody(functionBody, FunctionDecl::BodyStatus::Parsed);

  return status;
}

BraceStmt *Parser::ParseFunctionBodyImpl(ParsingDecl &parsingDecl,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

ParserResult<Decl> Parser::ParseStructDecl(ParsingDecl &parsingDecl) {

  ParserResult<Decl> result;
  ParsingScope structDeclScope(*this, ScopeKind::StructDecl,
                               "parsing struct-declaration");

  assert(parsingDecl.GetTypeSpecifierCollector().IsStruct() &&
         "Attempting to parse a struct without a struct declaration.");

  /// AddVaue

  // At this stage, only the pure modifier is allowed
  if (parsingDecl.GetTypeQualifierCollector().HasAny() &&
      !parsingDecl.GetTypeQualifierCollector().IsPure()) {
    // Do some logging
    return MakeParserError();
  }

  auto structLoc = parsingDecl.GetTypeSpecifierCollector().GetLoc();
  assert(structLoc.isValid());

  // At this point, we are expecting an identifier
  assert(curTok.IsIdentifierOrUnderscore() &&
         "Invalid struct declarator or identifier");

  return result;
}

ParserResult<Decl> Parser::ParseEnumDecl(ParsingDecl &parsingDecl) {
  ParserResult<Decl> result;

  ParsingScope enumDeclScope(*this, ScopeKind::EnumDecl,
                             "parsing enum-declaration");

  assert(parsingDecl.GetTypeSpecifierCollector().IsEnum() &&
         "Attempting to parse a struct without a struct declaration.");

  if (parsingDecl.GetTypeQualifierCollector().HasAny()) {
    return result;
  }

  // if(parsingDecl.GetTypeSpecifierCollector().HasAny()){
  //   // Log that a specifier is already present
  //   return result;
  // }

  // if (parsingDecl.GetTypeQualifierCollector().HasAny()) {
  //   // Log that enums are not allowed to have qualifiers
  //   return result;
  // }
  // parsingDecl.GetTypeSpecifierCollector().AddEnum(ConsumeToken());

  return result;
}

ParserResult<Decl> Parser::ParseInterfaceDecl(ParsingDecl &parsingDecl) {

  ParserResult<Decl> result;

  ParsingScope scope(*this, ScopeKind::InterfaceDecl,
                     "parsing interface-declaration");

  assert(parsingDecl.GetTypeSpecifierCollector().IsInterface() &&
         "Attempting to parse a struct without a struct declaration.");

  if (parsingDecl.GetTypeQualifierCollector().HasAny()) {
    return result;
  }
  return result;
}

ParserResult<Decl> Parser::ParseImportDecl(ParsingDecl &parsingDecl) {
  ParserResult<Decl> result;

  assert(parsingDecl.GetImportSpecifierCollector().HasImport() &&
         "Attempting to parse import without import declaration.");

  ParsingScope importDeclScope(*this, ScopeKind::ImportDecl,
                               "parsing import-declaration");

  return result;
}