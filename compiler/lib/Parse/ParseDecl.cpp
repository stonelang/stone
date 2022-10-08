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
    ParsingDeclCollector newCollector(*this, GetAttributeFactory());
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
    } else if (collector.GetTypeSpecifierCollector().IsStruct()) {
      result = ParseStructDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeSpecifierCollector().IsEnum()) {
      result = ParseEnumDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeSpecifierCollector().IsInterface()) {
      result = ParseInterfaceDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeSpecifierCollector().IsBasicType()) {
      result = ParseVarDecl(collector);
      goto EndParse;
    } else if (collector.GetTypeSpecifierCollector().IsAuto()) {
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

  assert(collector.GetTypeSpecifierCollector().HasTypeSpecifierKind() &&
         "Attempting to parse type-patterns without a type specified");

  // TODO: Significant improvement required here. This is a starter.
  // May just require the TypeCollecter to add Immutable on creation but remove
  // it if mutable is added

  if (!collector.GetTypeQualifierCollector().HasMutable()) {
    collector.GetTypeQualifierCollector().AddImmutable(SrcLoc());
  }

  CollectTypePatterns(collector);
  assert(collector.GetTypePatternCollector().HasTypePatterns() &&
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

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
    // We only allow pure on functions => non-member function
    if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
      // Do some logging
      return syn::MakeSyntaxError();
    }
  }

  if (collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()) {
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
  if (PeekNextToken().IsDoubleColon()) {
    collector.GetFunctionSpecifierCollector().AddIsMember();
  }

  if (!collector.GetFunctionSpecifierCollector().HasIsMember() &&
      collector.GetStorageSpecifierCollector().HasStatic()) {
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

  // Create the function
  auto funDecl =
      FunDeclFactory::Create(collector, sc, nullptr, GetCurDeclContext());
  assert(funDecl);

  // Very simple for the time being
  return syn::MakeSyntaxResult<Decl>(funDecl);

  // // TODO: Think about this part

  // funDecl->SetAccessLevel(collector.GetAccessLevel());
  // funDecl->SetFunLoc(funLoc);

  // // funDecl->SetTemplate...
  // // QualType *returnType = nullptr;
  // // DeclName fullName;

  // // Scope is functin signaure
  // if (ParseFunctionSignature(collector, *funDecl).IsError()) {
  //   return syn::MakeSyntaxError();
  // }

  // // Scope is now function body
  // status |= ParseFunctionBody(collector, *funDecl);
  // syn::VerifyDecl(funDecl);
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

    if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
      if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
        // TODO: Log
        status.SetIsError();
        return status;
      }
    }
    // We can call collect here to get the return type
    // collector.CollectUntil(tok::l_brace);
    // if (!collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()) {
    //   // Perform some logging function must return a function type
    //   status.SetIsError();
    //   return status;
    // }
    // TODO: Look for TypeSpecs

    // Collect Qualifiers

    // We are checking this here for now.
    if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
      if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
        status.SetHasCodeCompletion();
        return status;
      }
    }
    while (!GetTok().IsQualifier()) {
      CollectTypeQualifier(collector);
    }

    SyntaxResult<TypeRep> resultTypeRep = ParseDeclResultType(
        collector, diag::err_expected_type_for_function_result);
  }

  // status |= ParseFunctionResult(collector);
  // assert(curTok.Is(tok::arrow) && "Require '->'");
  // auto arrowLoc = ConsumeToken(tok::arrow);

  // ParseReturnType();

  // Parse the return type
  // funDecl->SetReturnType();

  // ConsumeToken();
  return status;
}
SyntaxStatus Parser::ParseFunctionArguments(ParsingDeclCollector &collector) {

  SrcLoc lParenLoc;
  SrcLoc rParenLoc;

  ParsingScope funArgScope(*this, ScopeKind::FunctionArguments,
                           "parsing fun arguments");

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

  assert(collector.GetTypeSpecifierCollector().IsStruct() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
    // We only allow pure on structs
    if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
      return result;
    }
  }

  auto structLoc = collector.GetTypeSpecifierCollector().GetLoc();
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

  assert(collector.GetTypeSpecifierCollector().IsEnum() &&
         "Attempting to parse a struct without a struct declaration.");

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
    return result;
  }

  // if(collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()){
  //   // Log that a specifier is already present
  //   return result;
  // }

  // if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
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

  assert(collector.GetTypeSpecifierCollector().IsInterface() &&
         "Attempting to parse a struct without a struct declaration.");

  ParsingScope enumDeclScope(*this, ScopeKind::UsingDecl,
                             "parsing enum-declaration");

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
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
