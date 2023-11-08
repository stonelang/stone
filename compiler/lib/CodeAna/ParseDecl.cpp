#include "stone/AST/Stmt.h"
#include "stone/Basic/CodeAnaDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/CodeAna/Parser.h"
#include "stone/CodeAna/Parsing.h"
// #include "stone/AST/Using.h"
#include "stone/AST/AST.h"
#include "stone/AST/ASTContext.h"

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
  while (IsParsing()) {
    // Parse a single top-level decl
    auto result = ParseTopLevelDecl();
    if (!Success(result)) {
      if (listener) {
        listener->OnError();
      }
      return;
    }
    if (listener) {
      listener->OnDecl(result.Get(), true);
    }
    results.push_back(result);
  }
}
// Ex: sample.stone
// fun F0() -> void {}
// fun F1() -> void {}
// There are two top decls - F0 and F1
// This call parses one at a time and adds it to the ASTFile
ParserResult<Decl> Parser::ParseTopLevelDecl() {

  assert(GetCurScope() == nullptr && "A scope is already active?");
  ParsingScope topLevelScope(*this, ASTScopeKind::TopLevel,
                             "parsing top-level declaration");

  return ParseDecl(ParsingDeclFlags::AllowTopLevel);
}

// NOTE: This is ripe for recursion.
ParserResult<Decl> Parser::ParseDecl(ParsingDeclOptions flags,
                                     ParsingDeclCollector *collector) {
  if (collector) {
    return ParseDeclInternal(*collector);
  } else {
    ParsingDeclCollector newCollector(*this);
    newCollector.flags = flags;
    return ParseDeclInternal(newCollector);
  }
}
/// Parse declaration specs
ParserResult<Decl> Parser::ParseDeclInternal(ParsingDeclCollector &collector) {

  ParserStatus status;
  ParserResult<Decl> result;
  ParsingScope declScope(*this, ASTScopeKind::Decl, "parsing declaration");

  while (result.IsNull() && IsParsing()) {
    /// Collect using(s), qualifier(s), and specifier.
    status |= CollectDecl(collector);
    if (status.HasCodeCompletion()) {
      goto EndParse;
    }
    status |= VerifyDeclCollected(collector);
    if (status.IsError()) {
      goto EndParse;
    }
    if (collector.GetUsingDeclarationCollector().HasUsing()) {
      result = ParseUsingDecl(collector);
      goto EndParse;
    } else if (collector.GetFunctionSpecifierCollector().HasFun()) {
      result = ParseFunDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeCollector()
                   .GetTypeSpecifierCollector()
                   .IsStruct()) {
      result = ParseStructDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeCollector()
                   .GetTypeSpecifierCollector()
                   .IsEnum()) {
      result = ParseEnumDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeCollector()
                   .GetTypeSpecifierCollector()
                   .IsInterface()) {
      result = ParseInterfaceDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeCollector()
                   .GetTypeSpecifierCollector()
                   .IsBasicType()) {
      result = ParseVarDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeCollector()
                   .GetTypeSpecifierCollector()
                   .IsAuto()) {
      result = ParseAutoDecl(collector);
      goto EndParse;
    }
  } // End of while

EndParse : {
  if (result.IsNull()) {
    // collector.PrintD();
    EndParsing();
  }
  return result;
}
}
// ParserStatus ParsingDeclCollector::CollectUntil(tok kind) {
//   ParserStatus status;
//   while (GetParser().GetTok().IsNot(kind)) {
//     status |= Collect();
//     if (status.HasCodeCompletion()) {
//       break;
//     }
//   }
//   return status;
// }

void Parser::ParseDeclName() {}
ParserResult<Decl> Parser::ParseVarDecl(ParsingDeclCollector &collector) {

  ParserResult<Decl> result;
  ParsingScope varDeclScope(*this, ASTScopeKind::VarDecl,
                            "parsing var declaration");

  assert(collector.GetTypeCollector().GetTypeSpecifierCollector().HasAny() &&
         "Attempting to parse type-patterns without a type specified");

  // TODO: Significant improvement required here. This is a starter.
  // May just require the TypeCollecter to add Immutable on creation but remove
  // it if mutable is added

  if (!collector.GetTypeCollector().GetTypeQualifierCollector().HasMutable()) {
    collector.GetTypeCollector().GetTypeQualifierCollector().AddImmutable(
        SrcLoc());
  }

  CollectTypeChunks(collector.GetTypeCollector());
  assert(collector.GetTypeCollector().GetTypeChunkCollector().HasAny() &&
         "Type is missing a type-pattern");

  auto varDecl = VarDecl::Create(GetASTContext());

  return result;
}

ParserResult<Decl> Parser::ParseAutoDecl(ParsingDeclCollector &collector) {

  ParserResult<Decl> result;
  ParsingScope autoDeclScope(*this, ASTScopeKind::AutoDecl,
                             "parsing auto storage declaration");

  // ParsingDeclaratorCollector declaratorCollector(collector,
  // DeclaratorASTScopeKind::Variable); auto status =
  // ParseDeclarator(declaratorCollector);

  return result;
}

ParserResult<Decl> Parser::ParseFunDecl(ParsingDeclCollector &collector) {

  ParsingScope funDeclScope(*this, ASTScopeKind::FunDecl,
                            "parsing fun declaration");

  assert(collector.GetFunctionSpecifierCollector().HasFun() &&
         "Attempting to parse a function without a functin definition.");

  assert(collector.GetFunctionSpecifierCollector().GetFunLoc().isValid());

  // At this stage, only the pure modifier is allowed
  if (collector.GetTypeCollector()
          .GetTypeQualifierCollector()
          .GetFastQuals()
          .HasAny() &&
      !collector.GetTypeCollector()
           .GetTypeQualifierCollector()
           .GetFastQuals()
           .HasPureOnly()) {
    // Do some logging
    return stone::MakeParserError();
  }

  if (collector.GetTypeCollector().GetTypeSpecifierCollector().HasAny()) {
    // TODO: Log a message -- not allowed to have type specs here
    return stone::MakeParserError();
  }

  // Make sure we have a valid identifier
  if (!GetTok().IsIdentifierOrUnderscore()) {
    // Do some logging  "Expecting function declarator or identifier");
    return stone::MakeParserError();
  }

  ParserStatus status;
  Identifier basicName;
  SrcLoc nameLoc;
  status = ParseIdentifier(basicName, nameLoc);

  // TODO: May want to move to ParseFunctionSignaure
  Identifier parentName;
  SrcLoc parentNameLoc;

  // TODO: You have to perform a name look-up where because you will be dealing
  // with "identifier::""
  if (GetTok().IsDoubleColon()) {
    if (collector.GetStorageSpecifierCollector().HasStatic()) {
      // TODO: Log
      return stone::MakeParserError();
    }
    // TODO: You are consuming the double colon
    collector.GetFunctionSpecifierCollector().AddIsMember(ConsumeToken());

    if (!GetTok().IsIdentifierOrUnderscore()) {
      // Do some logging  "Expecting Parent identifier");
      return stone::MakeParserError();
    }
    // TODO: That identifier should already exist
    // status = ParseIdentifier(parentName, parentNameLoc);
  }

  if (collector.GetStorageSpecifierCollector().HasStatic() &&
      collector.GetFunctionSpecifierCollector().HasIsMember()) {
    // Log only member functions can be status
    return stone::MakeParserError();
  }

  DeclName fullName;
  // Now, parse the function signature
  status |= ParseFunctionSignature(collector, basicName, fullName);

  if (status.IsError()) {
    return status;
  }

  collector.SetDeclName(fullName);
  collector.SetDeclNameLoc(nameLoc);

  // TODO: Ok for now.
  if (collector.GetTypeCollector().GetType().IsNull()) {
    status.SetIsError();
    return status;
  }

  // Apply what what collected
  // collector.Apply();
  // Create the function
  auto funDecl = FunDecl::Create(collector, sc, GetCurDeclContext());
  assert(funDecl);

  // TODO: Find a better place for this -- maybe pass as parameter
  if (collector.flags.contains(ParsingDeclFlags::AllowTopLevel)) {
    funDecl->SetIsTopLevel();
  }

  if (collector.GetFunctionSpecifierCollector().HasIsMember()) {
  }

  if (!status.HasCodeCompletion()) {
    status |= ParseFunctionBody(collector, *funDecl);
  }
  // Very simple for the time being
  return stone::MakeParserResult<Decl>(funDecl);
}

ParserStatus Parser::ParseFunctionSignature(ParsingDeclCollector &collector,
                                            Identifier basicName,
                                            DeclName &fullName) {
  ParserStatus status;
  ParsingScope funSigScope(*this, ASTScopeKind::FunctionSignature,
                           "parsing fun signature");

  status |= ParseFunctionArguments(collector);
  if (status.IsError()) {
    return status;
  }

  // TODO: simple for now
  fullName = DeclName(basicName);

  SrcLoc arrowLoc;
  if (GetTok().IsArrow()) {
    ParsingScope functionResult(*this, ASTScopeKind::ReturnClause,
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

    if (collector.GetTypeCollector()
            .GetTypeQualifierCollector()
            .GetFastQuals()
            .HasAny()) {
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
    CollectTypeQualifiers(collector.GetTypeCollector());

    // Why not just ParseFunctionType
    auto retType =
        ParseDeclResultType(collector.GetTypeCollector(),
                            diag::err_expected_type_for_function_result);
    collector.GetTypeCollector().SetType(retType);

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
ParserStatus Parser::ParseFunctionArguments(ParsingDeclCollector &collector) {

  SrcLoc lParenLoc;
  SrcLoc rParenLoc;

  // ParenTracker
  ParsingScope funArgScope(*this, ASTScopeKind::FunctionArguments,
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
    return stone::MakeParserError();
  }

  if (GetTok().IsRParen()) {
    lParenLoc = ConsumeToken(tok::r_paren);
  } else {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
    return stone::MakeParserError();
  }
  return stone::MakeParserSuccess();
}

ParserStatus Parser::ParseFunctionBody(ParsingDeclCollector &collector,
                                       FunctionDecl &funDecl) {

  // TODO:  BraceStmtPair braceStmtPair;

  // This is where you what to start a BracePairDelimeter
  ParserStatus status;
  ParsingScope funBodyScope(*this, ASTScopeKind::FunctionBody,
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

BraceStmt *Parser::ParseFunctionBodyImpl(ParsingDeclCollector &collector,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

ParserResult<Decl> Parser::ParseStructDecl(ParsingDeclCollector &collector) {

  ParserResult<Decl> result;
  ParsingScope structDeclScope(*this, ASTScopeKind::StructDecl,
                               "parsing struct-declaration");

  assert(collector.GetTypeCollector().GetTypeSpecifierCollector().IsStruct() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeCollector()
          .GetTypeQualifierCollector()
          .GetFastQuals()
          .HasAny()) {
    return stone::MakeParserError();
  }

  auto structLoc =
      collector.GetTypeCollector().GetTypeSpecifierCollector().GetLoc();
  assert(structLoc.isValid());

  // At this point, we are expecting an identifier
  assert(curTok.IsIdentifierOrUnderscore() &&
         "Invalid struct declarator or identifier");

  return result;
}

ParserResult<Decl> Parser::ParseEnumDecl(ParsingDeclCollector &collector) {
  ParserResult<Decl> result;

  ParsingScope enumDeclScope(*this, ASTScopeKind::EnumDecl,
                             "parsing enum-declaration");

  assert(collector.GetTypeCollector().GetTypeSpecifierCollector().IsEnum() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeCollector()
          .GetTypeQualifierCollector()
          .GetFastQuals()
          .HasAny()) {
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

ParserResult<Decl> Parser::ParseInterfaceDecl(ParsingDeclCollector &collector) {

  ParserResult<Decl> result;

  ParsingScope interfaceDeclScope(*this, ASTScopeKind::InterfaceDecl,
                                  "parsing interface-declaration");
  assert(
      collector.GetTypeCollector().GetTypeSpecifierCollector().IsInterface() &&
      "Attempting to parse a struct without a struct declaration.");

  ParsingScope enumDeclScope(*this, ASTScopeKind::UsingDecl,
                             "parsing enum-declaration");

  if (collector.GetTypeCollector()
          .GetTypeQualifierCollector()
          .GetFastQuals()
          .HasAny()) {
    return result;
  }
  return result;
}

ParserResult<Decl> Parser::ParseUsingDecl(ParsingDeclCollector &collector) {
  ParserResult<Decl> result;

  assert(collector.GetUsingDeclarationCollector().HasUsing() &&
         "Attempting to parse a function without a functin definition.");

  return result;
}
