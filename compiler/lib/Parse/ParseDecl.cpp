#include "stone/Basic/Defer.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsStartOfDecl(const Token &curTok) {
  switch (curTok.GetKind()) {
  case tok::kw_interface:
  case tok::kw_fun:
  case tok::kw_inline:
  case tok::kw_struct:
  case tok::kw_space:
  case tok::kw_const:
  case tok::kw_public:
  case tok::kw_private:
  case tok::kw_internal:
  case tok::kw_static:
  case tok::kw_forward:
    return true;
  default:
    return false;
  }
}
void Parser::ParseTopLevelDecls(
    llvm::SmallVector<SyntaxResult<Decl>> &results) {
  // Prime the Parser's curTok
  // The Lexer has the first curTok but the Parser's curTok defaults to tk::MAX
  // So, update the parser's curTok with the first curTok from the Lexer
  if (curTok.Is(tok::MAX)) {
    ConsumeToken();
  }
  while (!IsDone()) {
    // Parse a single top-level decl
    auto result = ParseTopLevelDecl();
    if (listener) {
      if (HasError()) {
        listener->OnError();
        return;
      } else {
        listener->OnDecl(result.Get(), true);
      }
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
  assert(IsStartOfDecl(curTok) && "Invalid top-declaration");

  assert(GetCurScope() == nullptr && "A scope is already active?");

  // TODO: Get scope description from scopdeid::parsing_top_level_declaration
  ScopeContext parsingTopLevelDecl(*this, ScopeKind::SyntaxFile,
                                   "parsing top-level declaration");

  return ParseDecl(ParsingDeclFlags::AllowTopLevel,
                   [&](Decl *d) { /* Do nothing for now*/ });
}

// NOTE: This is ripe for recursion.
SyntaxResult<Decl> Parser::ParseDecl(ParsingDeclOptions flags,
                                     llvm::function_ref<void(Decl *)> handler) {

  ParsingDeclSpecifier spec(*this, GetAttributeFactory());
  spec.flags = flags;

  return ParseDecl(spec, handler);
}

/// Parse declaration specs
SyntaxResult<Decl> Parser::ParseDecl(ParsingDeclSpecifier &spec,
                                     llvm::function_ref<void(Decl *)> handler) {
  SyntaxStatus status;

  SyntaxResult<Decl> result;
  // TODO: Replace with ParsingScope
  ScopeContext parsingDecl(*this, ScopeKind::Decl, "parsing declaration");

  while (true) {
  BeginParse:
    if (IsDone()) {
      goto EndParse;
    }
    // First, we check for access levels -- if one is not found, we will
    // eventually come back to it.
    if (curTok.IsAccessLevel()) {
      if (!spec.GetAccessLevelContext().HasAccessLevel()) {
        status = ParseAccessLevel(spec.GetAccessLevelContext());
        if (status.hasCodeCompletion() && status.IsSuccess()) {
          goto BeginParse;
        }
      } else {
        /// PrintD -- found dup
        goto EndParse;
      }
    } else if (curTok.IsQualifier()) {
      if (!spec.GetTypeQualifierCollector().HasAllTypeQualifiers()) {
        status = ParseTypeQualifiers(spec.GetTypeQualifierCollector());
        if (status.hasCodeCompletion() && status.IsSuccess()) {
          goto BeginParse;
        }
      } else {
        /// PrintD
        goto EndParse;
      }
    } else if (curTok.IsStruct()) {
      if (!spec.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        spec.GetTypeSpecifierContext().AddStruct(ConsumeToken());
        result = ParseStructDecl(spec);

      } else {
        // PrintD -- dup
      }
      goto EndParse;
    } else if (curTok.IsInterface()) {
      if (!spec.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        spec.GetTypeSpecifierContext().AddInterface(ConsumeToken());
        result = ParseInterfaceDecl(spec);
      } else {
        // PrintD
      }
      goto EndParse;
    } else if (curTok.IsEnum()) {
      if (!spec.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        spec.GetTypeSpecifierContext().AddEnum(ConsumeToken());
        result = ParseInterfaceDecl(spec);
      } else {
        // PrintD
      }
      goto EndParse;
    } else if (IsBasicType(curTok.GetKind())) {
      if (!spec.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        status = ParseBasicTypeSpecifier(spec.GetTypeSpecifierContext());
        if (status.hasCodeCompletion() && status.IsSuccess()) {
        }
      } else {
        // PrintD
      }
      goto BeginParse;
    } else if (curTok.IsPointerOperator()) {
      if (spec.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        // spec.GetTypeSpecifierContext().Bits.IsPointer = true;
        ConsumeToken();
        goto BeginParse;
      } else {
        // PrinD -- random varialb
        goto EndParse;
      }
    } else if (curTok.IsFun()) {
      if (!spec.GetFunctionSpecifierContext().HasFun()) {
        spec.GetFunctionSpecifierContext().AddFun(ConsumeToken());
        result = ParseFunDecl(spec);
      } else {
        // PrintD
      }
      goto EndParse;
    } else {
      if (curTok.IsIdentifierOrUnderscore()) {
        if (spec.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
          ParsingDeclarator declarator(spec, DeclaratorContextKind::SyntaxFile);
          result = ParseVarDecl(declarator);
        } else {
          // PrintD
          goto EndParse;
        }
      }
    }
    ConsumeToken();

  } // End of while

EndParse : {
  // while (!IsDone()) {
  //   ConsumeToken();
  // }
  return result;
}
}

SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDeclarator &declarator) {

  SyntaxResult<Decl> result;

  /// This is where you will call ParseType
  // assert(curTok.IsIdentifierOrUnderscore() && "Invalid identifier");
  // assert(declarator.GetParsingDeclSpecifier()
  //            .GetTypeSpecifierContext()
  //            .HasTypeSpecifierKind() &&
  //        "Declarator does not have a type");

  // We are dealing with a pointer operator and:

  return result;
}

SyntaxResult<Decl> Parser::ParseFunDecl(ParsingDeclSpecifier &spec) {

  SyntaxResult<Decl> result;

  assert(spec.GetFunctionSpecifierContext().HasFun() &&
         "Attempting to parse a function without a functin definition.");

  auto funLoc = spec.GetFunctionSpecifierContext().GetFunLoc();

  // At this point, we are expecting an identifier
  assert(curTok.IsIdentifierOrUnderscore() &&
         "Expecting function declarator or identifier");

  ScopeContext parsingFunDecl(*this, ScopeKind::FunctionDecl,
                              "parsing fun declaration");

  // Build the DeclName
  DeclNameInfo nameInfo;

  // Parse function name.
  auto name = GetIdentifier(curTok.GetText());
  DeclName fullName(&name);
  nameInfo.SetName(fullName);

  // very simple for now.
  SrcLoc nameLoc = ConsumeToken(tok::identifier);
  nameInfo.SetNameLoc(nameLoc);

  if (PeekNextToken().IsDoubleColon()) {
    spec.GetFunctionSpecifierContext().AddIsMember();
  }

  SyntaxStatus status;
  // Now, parse the function signature
  status |= ParseFunctionSignature(nameInfo, spec);

  // FunDecl *funDecl = syn.MakeFunDecl(nameInfo, sc);
  // // TODO: Think about this part

  // funDecl->SetAccessLevel(spec.GetAccessLevel());
  // funDecl->SetFunLoc(funLoc);

  // // funDecl->SetTemplate...
  // // QualType *returnType = nullptr;
  // // DeclName fullName;

  // // Scope is functin signaure
  // if (ParseFunctionSignature(spec, *funDecl).IsError()) {
  //   return syn::MakeSyntaxError();
  // }

  // // Scope is now function body
  // status |= ParseFunctionBody(spec, *funDecl);
  // syn::VerifyDecl(funDecl);
  return result;
}

SyntaxStatus Parser::ParseFunctionSignature(const DeclNameInfo &nameInfo,
                                            ParsingDeclSpecifier &spec) {
  SyntaxStatus status;
  // ParsingScope syntaxScope(SyntaxKind::FunctionSignature);

  ScopeContext parsingFunSig(*this, ScopeKind::FunctionSignature,
                             "parsing fun signature");

  // TODO:
  // if(name == "Main"){
  //   sf.SetHasMainFun(true)
  // }

  status |= ParseFunctionArguments(spec);

  // status |= ParseFunctionResult(spec);

  // assert(curTok.Is(tok::arrow) && "Require '->'");
  // auto arrowLoc = ConsumeToken(tok::arrow);

  // ParseReturnType();

  // Parse the return type
  // funDecl->SetReturnType();

  SyntaxResult<QualType> resultType =
      ParseDeclResultType(spec.GetTypeSpecifierContext(),
                          diag::err_expected_type_for_function_result);

  // ConsumeToken();
  return status;
}
SyntaxStatus Parser::ParseFunctionArguments(ParsingDeclSpecifier &spec) {

  SrcLoc lParenLoc;
  SrcLoc rParenLoc;

  if (curTok.IsLeftParen()) {
    lParenLoc = ConsumeToken(tok::l_paren);
  } else {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
    return syn::MakeSyntaxError();
  }

  if (curTok.IsRightParen()) {
    lParenLoc = ConsumeToken(tok::r_paren);

  } else {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
    return syn::MakeSyntaxError();
  }
  return syn::MakeSyntaxSuccess();
}

// TODO: Actually return the type;
// SyntaxStatus Parser::ParseFunctionResult(ParsingDeclSpecifier &spec) {
//   SyntaxStatus status;

//   SrcLoc arrowLoc;
//   if (!ConsumeIf(tok::arrow, arrowLoc)) {
//     // FixIt ':' to '->'.
//     PrintD(curTok, diag::err_expected_arrow_after_function_param)
//         .WithFix()
//         .Replace(curTok.GetLoc(), llvm::StringRef("->"));

//     // arrowLoc = ConsumeToken(tok::colon);
//   }

//   // ParseDeclSpecifier();

//   return status;
// }

SyntaxStatus Parser::ParseFunctionBody(ParsingDeclSpecifier &spec,
                                       FunctionDecl &funDecl) {

  SyntaxStatus status;
  assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  auto lParenLoc = ConsumeToken(tok::l_brace);

  assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  auto rParenLoc = ConsumeToken(tok::r_brace);

  return status;
}

BraceStmt *Parser::ParseFunctionBodyImpl(ParsingDeclSpecifier &spec,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

SyntaxResult<Decl> Parser::ParseStructDecl(ParsingDeclSpecifier &spec) {

  return syn::MakeSyntaxResult<Decl>(nullptr);
}

SyntaxResult<Decl> Parser::ParseEnumDecl(ParsingDeclSpecifier &specifier) {
  return syn::MakeSyntaxResult<Decl>(nullptr);
}
SyntaxResult<Decl> Parser::ParseInterfaceDecl(ParsingDeclSpecifier &specifier) {

  return syn::MakeSyntaxResult<Decl>(nullptr);
}
