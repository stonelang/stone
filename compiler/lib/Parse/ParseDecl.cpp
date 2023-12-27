#include "stone/Basic/Defer.h"
#include "stone/Diag/ASTDiagnostic.h"
#include "stone/IDE.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/Stmt.h"
// #include "stone/Syntax/Using.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTNode.h"
#include "stone/Syntax/Stmt.h"

using namespace stone;

void Parser::ParseTopLevelDecls(
    llvm::SmallVector<SyntaxResult<Decl>> &results) {
  // Prime the Parser's curTok
  // The Lexer has the first curTok but the Parser's curTok defaults to tk::MAX
  // So, update the parser's curTok with the first curTok from the Lexer
  if (curTok.Is(tok::MAX)) {
    ConsumeToken();
  }
  auto Success = [&](SyntaxResult<Decl> &result) -> bool {
    return (!result.IsError() && !HasError() && result.IsNonNull());
  };
  while (IsParsing() && IsTopLevelDeclSpecifier()) {
    ParsingDecl collector(*this);
    collector.parsingDeclOpts = ParsingDeclFlags::AllowTopLevel;
    auto result = ParseTopLevelDecl(collector);
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
SyntaxResult<Decl> Parser::ParseTopLevelDecl(ParsingDecl &collector) {

  assert(GetCurScope() == nullptr && "A scope is already active?");
  ParsingScope parseTopLevelDeclScope(*this, ScopeKind::TopLevel,
                                      "parsing top-level declaration");
  SyntaxResult<Decl> result;
  while (result.IsNull() && IsParsing() && IsTopLevelDeclSpecifier()) {
    if (CollectDeclSpecifier(collector).HasCodeCompletion()) {
      // This is an empty file -- stop parsing.
      return result;
    }
    result = ParseDecl(collector);
  }
  return result;
}

/// Parse declaration specs
SyntaxResult<Decl> Parser::ParseDecl(ParsingDecl &collector) {

  ParsingScope parseDeclScope(*this, ScopeKind::Decl, "parsing declaration");

  if (collector.GetImportSpecifierCollector().HasImport()) {
    return ParseImportDecl(collector);
  } else if (collector.GetFunctionSpecifierCollector().HasFun()) {
    return ParseFunDecl(collector);
  } else if (collector.GetTypeSpecifierCollector().IsStruct()) {
    return ParseStructDecl(collector);
  } else if (collector.GetTypeSpecifierCollector().IsEnum()) {
    return ParseEnumDecl(collector);
  } else if (collector.GetTypeSpecifierCollector().IsInterface()) {
    return ParseInterfaceDecl(collector);
  } else if (collector.GetTypeSpecifierCollector().IsAuto()) {
    return ParseAutoDecl(collector);
  }
  return SyntaxResult<Decl>();
}
bool Parser::IsTopLevelDeclSpecifier() { return GetTok().IsTopLevel(); }

void Parser::ParseDeclName() {}

SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDecl &collector) {

  SyntaxResult<Decl> result;
  ParsingScope varDeclScope(*this, ScopeKind::VarDecl,
                            "parsing var declaration");

  assert(collector.GetTypeSpecifierCollector().HasAny() &&
         "Attempting to parse type-patterns without a type specified");

  // TODO: Significant improvement required here. This is a starter.
  // May just require the TypeQualifierCollector to add 'final' on creation but
  // remove it if mutable is added

  if (!collector.GetTypeQualifierCollector().HasAny()) {
    collector.GetTypeQualifierCollector().AddFinal(SrcLoc());
  }

  CollectTypeChunks(collector);

  assert(collector.GetTypeChunkCollector().HasAny() &&
         "Type is missing a type-thunk");

  auto varDecl = VarDecl::Create(GetASTContext());

  return result;
}

SyntaxResult<Decl> Parser::ParseAutoDecl(ParsingDecl &collector) {

  SyntaxResult<Decl> result;
  ParsingScope autoDeclScope(*this, ScopeKind::AutoDecl,
                             "parsing auto storage declaration");

  // ParsingDeclaratorCollector declaratorCollector(collector,
  // DeclaratorScopeKind::Variable); auto status =
  // ParseDeclarator(declaratorCollector);

  return result;
}

SyntaxResult<Decl> Parser::ParseFunDecl(ParsingDecl &collector) {

  ParsingScope funDeclScope(*this, ScopeKind::FunDecl,
                            "parsing fun declaration");

  assert(collector.GetFunctionSpecifierCollector().HasFun() &&
         "Attempting to parse a function without a functin definition.");

  assert(collector.GetFunctionSpecifierCollector().GetFunLoc().isValid());

  // TODO: Here or in ParseFunctionType
  collector.GetTypeChunkCollector().AddFunction();

  // At this stage, only the pure modifier is allowed
  if (collector.GetTypeQualifierCollector().HasAny() &&
      !collector.GetTypeQualifierCollector().IsPure()) {
    // Do some logging
    return MakeSyntaxError();
  }

  if (collector.GetTypeSpecifierCollector().HasAny()) {
    // TODO: Log a message -- not allowed to have type specs here
    return MakeSyntaxError();
  }

  // Make sure we have a valid identifier
  if (!GetTok().IsIdentifierOrUnderscore()) {
    // Do some logging  "Expecting function declarator or identifier");
    return MakeSyntaxError();
  }

  SyntaxStatus status;
  Identifier basicName;
  SrcLoc nameLoc;
  status = ParseIdentifier(basicName, nameLoc);

  // TODO: May want to move to ParseFunctionSignaure
  Identifier parentName;
  SrcLoc parentNameLoc;

  // TODO: You have to perform a name look-up where because you will be
  // dealing with "identifier::""
  if (GetTok().IsDoubleColon()) {
    if (collector.GetStorageSpecifierCollector().HasStatic()) {
      // TODO: Log
      return MakeSyntaxError();
    }
    // TODO: You are consuming the double colon
    collector.GetFunctionSpecifierCollector().AddIsMember(ConsumeToken());

    if (!GetTok().IsIdentifierOrUnderscore()) {
      // Do some logging  "Expecting Parent identifier");
      return MakeSyntaxError();
    }
    // TODO: That identifier should already exist
    // status = ParseIdentifier(parentName, parentNameLoc);
  }

  if (collector.GetStorageSpecifierCollector().HasStatic() &&
      collector.GetFunctionSpecifierCollector().HasIsMember()) {
    // Log only member functions can be status
    return MakeSyntaxError();
  }

  DeclName fullName;
  // Now, parse the function signature
  status |= ParseFunctionSignature(collector, basicName, fullName);

  if (status.IsError()) {
    return status;
  }

  collector.GetDeclNameCollector().SetName(fullName);
  collector.GetDeclNameCollector().SetLoc(nameLoc);

  // TODO: Ok for now.
  if (collector.GetTypeSpecifierCollector().GetType().IsNull()) {
    status.SetIsError();
    return status;
  }

  // Apply what what collected
  // collector.Apply();
  // Create the function
  auto funDecl = FunDecl::Create(collector, astContext, GetCurDeclContext());
  assert(funDecl);

  // TODO: Find a better place for this -- maybe pass as parameter
  if (collector.parsingDeclOpts.contains(ParsingDeclFlags::AllowTopLevel)) {
    funDecl->SetIsTopLevel();
  }

  if (collector.GetFunctionSpecifierCollector().HasIsMember()) {
  }

  if (!status.HasCodeCompletion()) {
    status |= ParseFunctionBody(collector, *funDecl);
  }
  // Very simple for the time being
  return stone::MakeSyntaxResult<Decl>(funDecl);
}

SyntaxStatus Parser::ParseFunctionSignature(ParsingDecl &collector,
                                            Identifier basicName,
                                            DeclName &fullName) {
  SyntaxStatus status;
  ParsingScope funSigScope(*this, ScopeKind::FunctionSignature,
                           "parsing fun signature");

  status |= ParseFunctionArguments(collector);
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
      PrintD(curTok, diag::err_expected_arrow_after_function_param)
          .WithFix()
          .Replace(curTok.GetLoc(), llvm::StringRef("->"));
      // arrowLoc = ConsumeToken(tok::colon);
    } else {
      collector.GetFunctionSpecifierCollector().AddArrowLoc(arrowLoc);
    }

    if (collector.GetTypeQualifierCollector().HasAny()) {
      // if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
      //   // TODO: Log
      //   status.SetHasCodeCompletion();
      //   return status;
      // }

      status.SetHasCodeCompletion();
      return status;
    }
    // We can call collect here to get the return type
    // collector.CollectUntil(tok::l_brace);
    // if (!collector.GetTypeSpecifierCollector().HasAny()) {
    //   // Perform some logging function must return a function type
    //   status.SetIsError();
    //   return status;
    // }
    // TODO: Look for TypeSpecs

    // Collect Qualifiers
    CollectTypeQualifiers(collector);

    // Why not just ParseFunctionType
    auto retType = ParseDeclResultType(
        collector, diag::err_expected_type_for_function_result);

    collector.GetTypeSpecifierCollector().SetType(retType);

    // status |= ParseFunctionResult(collector);
    // assert(curTok.Is(tok::arrow) && "Require '->'");
    // auto arrowLoc = ConsumeToken(tok::arrow);

    // ParseReturnType();

    // Parse the return type
    // funDecl->SetReturnType();

    // ConsumeToken();
    return status;
  }
}
SyntaxStatus Parser::ParseFunctionArguments(ParsingDecl &collector) {

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
    // auto diag = PrintD(Tok, diagID);
    return MakeSyntaxError();
  }

  if (GetTok().IsRParen()) {
    lParenLoc = ConsumeToken(tok::r_paren);
  } else {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
    return MakeSyntaxError();
  }
  return MakeSyntaxSuccess();
}

SyntaxStatus Parser::ParseFunctionBody(ParsingDecl &collector,
                                       FunctionDecl &funDecl) {

  // TODO:  BraceStmtPair braceStmtPair;

  // This is where you what to start a BracePairDelimeter
  SyntaxStatus status;
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

BraceStmt *Parser::ParseFunctionBodyImpl(ParsingDecl &collector,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

SyntaxResult<Decl> Parser::ParseStructDecl(ParsingDecl &collector) {

  SyntaxResult<Decl> result;
  ParsingScope structDeclScope(*this, ScopeKind::StructDecl,
                               "parsing struct-declaration");

  assert(collector.GetTypeSpecifierCollector().IsStruct() &&
         "Attempting to parse a struct without a struct declaration.");

  /// AddVaue

  // At this stage, only the pure modifier is allowed
  if (collector.GetTypeQualifierCollector().HasAny() &&
      !collector.GetTypeQualifierCollector().IsPure()) {
    // Do some logging
    return MakeSyntaxError();
  }

  auto structLoc = collector.GetTypeSpecifierCollector().GetLoc();
  assert(structLoc.isValid());

  // At this point, we are expecting an identifier
  assert(curTok.IsIdentifierOrUnderscore() &&
         "Invalid struct declarator or identifier");

  return result;
}

SyntaxResult<Decl> Parser::ParseEnumDecl(ParsingDecl &collector) {
  SyntaxResult<Decl> result;

  ParsingScope enumDeclScope(*this, ScopeKind::EnumDecl,
                             "parsing enum-declaration");

  assert(collector.GetTypeSpecifierCollector().IsEnum() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeQualifierCollector().HasAny()) {
    return result;
  }

  // if(collector.GetTypeSpecifierCollector().HasAny()){
  //   // Log that a specifier is already present
  //   return result;
  // }

  // if (collector.GetTypeQualifierCollector().HasAny()) {
  //   // Log that enums are not allowed to have qualifiers
  //   return result;
  // }
  // collector.GetTypeSpecifierCollector().AddEnum(ConsumeToken());

  return result;
}

SyntaxResult<Decl> Parser::ParseInterfaceDecl(ParsingDecl &collector) {

  SyntaxResult<Decl> result;

  ParsingScope scope(*this, ScopeKind::InterfaceDecl,
                     "parsing interface-declaration");

  assert(collector.GetTypeSpecifierCollector().IsInterface() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeQualifierCollector().HasAny()) {
    return result;
  }
  return result;
}

SyntaxResult<Decl> Parser::ParseImportDecl(ParsingDecl &collector) {
  SyntaxResult<Decl> result;

  assert(collector.GetImportSpecifierCollector().HasImport() &&
         "Attempting to parse import without import declaration.");

  ParsingScope importDeclScope(*this, ScopeKind::ImportDecl,
                               "parsing import-declaration");

  return result;
}
