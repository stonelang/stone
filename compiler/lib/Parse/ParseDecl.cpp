#include "stone/Basic/Defer.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Parse/Parsing.h"
#include "stone/Syntax/Stmt.h"
// #include "stone/Syntax/Using.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"
#include "stone/Syntax/SyntaxNode.h"

using namespace stone;
using namespace stone::syn;

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
// This call parses one at a time and adds it to the SyntaxFile
SyntaxResult<Decl> Parser::ParseTopLevelDecl() {

  assert(GetCurScope() == nullptr && "A scope is already active?");
  ParsingScope topLevelScope(*this, ScopeKind::TopLevel,
                             "parsing top-level declaration");

  return ParseDecl(ParsingDeclFlags::AllowTopLevel);
}

// NOTE: This is ripe for recursion.
SyntaxResult<Decl> Parser::ParseDecl(ParsingDeclOptions flags,
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
SyntaxResult<Decl> Parser::ParseDeclInternal(ParsingDeclCollector &collector) {

  SyntaxStatus status;
  SyntaxResult<Decl> result;
  ParsingScope declScope(*this, ScopeKind::Decl, "parsing declaration");

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
// SyntaxStatus ParsingDeclCollector::CollectUntil(tok kind) {
//   SyntaxStatus status;
//   while (GetParser().GetTok().IsNot(kind)) {
//     status |= Collect();
//     if (status.HasCodeCompletion()) {
//       break;
//     }
//   }
//   return status;
// }

void Parser::ParseDeclName() {}
SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;
  ParsingScope varDeclScope(*this, ScopeKind::VarDecl,
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

  auto varDecl = VarDeclFactory::Create(GetSyntaxContext());

  return result;
}

SyntaxResult<Decl> Parser::ParseAutoDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;
  ParsingScope autoDeclScope(*this, ScopeKind::AutoDecl,
                             "parsing auto storage declaration");

  // ParsingDeclaratorCollector declaratorCollector(collector,
  // DeclaratorScopeKind::Variable); auto status =
  // ParseDeclarator(declaratorCollector);

  return result;
}

SyntaxResult<Decl> Parser::ParseFunDecl(ParsingDeclCollector &collector) {

  ParsingScope funDeclScope(*this, ScopeKind::FunDecl,
                            "parsing fun declaration");

  assert(collector.GetFunctionSpecifierCollector().HasFun() &&
         "Attempting to parse a function without a functin definition.");

  assert(collector.GetFunctionSpecifierCollector().GetFunLoc().isValid());

  if (collector.GetTypeCollector().GetTypeQualifierCollector().HasAny()) {

    // We only allow pure on functions => non-member function
    // if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
    //   // Do some logging
    //   return syn::MakeSyntaxError();
    // }
    return syn::MakeSyntaxError();
  }

  if (collector.GetTypeCollector().GetTypeSpecifierCollector().HasAny()) {
    // TODO: Log a message -- not allowed to have type specs here
    return syn::MakeSyntaxError();
  }

  // ParsingDeclaratorCollector declaratorCollector(collector);
  // auto status = ParseDeclarator(declaratorCollector);

  if (!GetTok().IsIdentifierOrUnderscore()) {
    // Do some logging  "Expecting function declarator or identifier");
    return syn::MakeSyntaxError();
  }

  SyntaxStatus status;
  Identifier basicName;
  SrcLoc nameLoc;
  status = ParseIdentifier(basicName, nameLoc);

  // TODO: May want to move to ParseFunctionSignaure
  Identifier parentName;
  SrcLoc parentNameLoc;

  if (GetTok().IsDoubleColon()) {
    if (collector.GetStorageSpecifierCollector().HasStatic()) {
      // TODO: Log
      return syn::MakeSyntaxError();
    }
    collector.GetFunctionSpecifierCollector().AddIsMember(ConsumeToken());
    if (!GetTok().IsIdentifierOrUnderscore()) {
      // Do some logging  "Expecting Parent identifier");
      return syn::MakeSyntaxError();
    }
    status = ParseIdentifier(parentName, parentNameLoc);
  }

  if (collector.GetStorageSpecifierCollector().HasStatic() &&
      collector.GetFunctionSpecifierCollector().HasIsMember()) {
    // Log only member functions can be status
    return syn::MakeSyntaxError();
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
  auto funDecl = FunDeclFactory::Create(collector, sc, GetCurDeclContext());
  assert(funDecl);

  if (!status.HasCodeCompletion()) {
    status |= ParseFunctionBody(collector, *funDecl);
  }
  // Very simple for the time being
  return syn::MakeSyntaxResult<Decl>(funDecl);
}

SyntaxStatus Parser::ParseFunctionSignature(ParsingDeclCollector &collector,
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

    if (collector.GetTypeCollector().GetTypeQualifierCollector().HasAny()) {
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
    while (!GetTok().IsQualifier()) {
      CollectTypeQualifier(collector.GetTypeCollector());
    }

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
SyntaxStatus Parser::ParseFunctionArguments(ParsingDeclCollector &collector) {

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
    return syn::MakeSyntaxError();
  }

  if (GetTok().IsRParen()) {
    lParenLoc = ConsumeToken(tok::r_paren);
  } else {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
    return syn::MakeSyntaxError();
  }
  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::ParseFunctionBody(ParsingDeclCollector &collector,
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
      BraceStmtFactory::Create(lParenLoc, {}, rParenLoc, GetSyntaxContext());
  funDecl.SetBody(functionBody, FunctionDecl::BodyStatus::Parsed);

  return status;
}

BraceStmt *Parser::ParseFunctionBodyImpl(ParsingDeclCollector &collector,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

SyntaxResult<Decl> Parser::ParseStructDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;
  ParsingScope structDeclScope(*this, ScopeKind::StructDecl,
                               "parsing struct-declaration");

  assert(collector.GetTypeCollector().GetTypeSpecifierCollector().IsStruct() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeCollector().GetTypeQualifierCollector().HasAny()) {
    return syn::MakeSyntaxError();
  }

  auto structLoc =
      collector.GetTypeCollector().GetTypeSpecifierCollector().GetLoc();
  assert(structLoc.isValid());

  // At this point, we are expecting an identifier
  assert(curTok.IsIdentifierOrUnderscore() &&
         "Invalid struct declarator or identifier");

  return result;
}

SyntaxResult<Decl> Parser::ParseEnumDecl(ParsingDeclCollector &collector) {
  SyntaxResult<Decl> result;

  ParsingScope enumDeclScope(*this, ScopeKind::EnumDecl,
                             "parsing enum-declaration");

  assert(collector.GetTypeCollector().GetTypeSpecifierCollector().IsEnum() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeCollector().GetTypeQualifierCollector().HasAny()) {
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

SyntaxResult<Decl> Parser::ParseInterfaceDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;

  ParsingScope interfaceDeclScope(*this, ScopeKind::InterfaceDecl,
                                  "parsing interface-declaration");
  assert(
      collector.GetTypeCollector().GetTypeSpecifierCollector().IsInterface() &&
      "Attempting to parse a struct without a struct declaration.");

  ParsingScope enumDeclScope(*this, ScopeKind::UsingDecl,
                             "parsing enum-declaration");

  if (collector.GetTypeCollector().GetTypeQualifierCollector().HasAny()) {
    return result;
  }
  return result;
}

SyntaxResult<Decl> Parser::ParseUsingDecl(ParsingDeclCollector &collector) {
  SyntaxResult<Decl> result;

  assert(collector.GetUsingDeclarationCollector().HasUsing() &&
         "Attempting to parse a function without a functin definition.");

  return result;
}
