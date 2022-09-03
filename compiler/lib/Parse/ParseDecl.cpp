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
  assert(IsStartOfDecl(curTok) && "Invalid start of or top level declaration");

  return ParseDecl(ParsingDeclFlags::AllowTopLevel,
                   [&](Decl *d) { /* Do nothing for now*/ });
}

// NOTE: This is ripe for recursion.
SyntaxResult<Decl> Parser::ParseDecl(ParsingDeclOptions flags,
                                     llvm::function_ref<void(Decl *)> handler) {

  ParsingDeclSpecifier specifier(*this, GetAttributeFactory());
  specifier.flags = flags;

  return ParseDecl(specifier, handler);
}

/// Parse declaration specifiers
SyntaxResult<Decl> Parser::ParseDecl(ParsingDeclSpecifier &specifier,
                                     llvm::function_ref<void(Decl *)> handler) {

  SyntaxResult<Decl> result;
  // TODO: Replace with ParsingScope
  ParsingContext parsingContext(ParsingContextKind::Decl);
  // The ordering does not matter becuase the compiler will eventuall
  // order things in a nice way -- type is just the build up of the decl
  while (true) {
  BeginParse:

    if (IsDone()) {
      goto EndParse;
    }
    /// Look for any access specifier: public, internal, or private. Default to
    /// private.
    if (ParseAccessLevel(specifier)) {
      ConsumeToken();
      goto BeginParse;
    } else {
      specifier.AddPrivateAccessLevel(GetLoc());
      goto BeginParse;
    }
    /// Look for any type qualifiers: const, volatile, restrict, etc.
    if (ParseTypeQualifiers(specifier.GetTypeQualifireContext())) {
      ConsumeToken();
      goto BeginParse;
    }
    /// Look for any basic type specifiers : int, float, ..., etc.
    if (ParseBasicTypeSpecifier(specifier.GetTypeSpecifierContext())) {
      ConsumeToken();
      goto BeginParse;
    }
    /// Look for identifiers. If we find one, there must be a corresponding
    /// type.
    if (curTok.IsIdentifierOrUnderscore()) {
      if (specifier.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        ParsingDeclarator declarator(specifier,
                                     DeclaratorContextKind::SyntaxFile);
        result = ParseVarDecl(declarator);
      } else {
        // This is just some random variable with no type -- error message.
      }
      goto EndParse;
    }
    /// Check for function specifiers
    if (curTok.IsInline()) {
      specifier.GetFunctionSpecifierContext().AddInline(GetLoc());
      ConsumeToken();
      goto BeginParse;
    }
    if (curTok.IsFun()) {
      specifier.GetFunctionSpecifierContext().AddFunctionDef(GetLoc());
      if (PeekNextToken().IsDoubleColon()) {
        // specifier.GetFunctionSpecifierContext().SetIsMember();
      }
      result = ParseFunDecl(specifier);
      goto EndParse;
    }
    if (curTok.IsStruct()) {
      specifier.GetTypeSpecifierContext().AddStruct(GetLoc());
      result = ParseStructDecl(specifier);
      goto EndParse;
    }
    if (curTok.IsInterface()) {
      specifier.GetTypeSpecifierContext().AddInterface(GetLoc());
      // result = ParseInterfaceDecl(specifier);
      goto EndParse;
    }
    ConsumeToken();
  } // End of while

EndParse:
  return result;
}

SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDeclarator &declarator) {

  // assert(specifier.GetTypeSpecifierContext().HasTypeSpecifierKind());
  // const ParsingDeclSpecifier &specifier =
  //     declarator.GetParsingDeclSpecifier();

  SyntaxResult<Decl> result;

  if (!curTok.IsPointerOperator()) {
    return result;
  }
  if (curTok.IsPointerOperator()) {
  }
  // We are dealing with a pointer operator and:

  return result;
}

SyntaxResult<Decl> Parser::ParseFunDecl(ParsingDeclSpecifier &specifier) {

  assert(curTok.IsFun() &&
         "Attempting to parse a 'fun' decl with incorrect curTok.");
  auto funLoc = ConsumeToken(tok::kw_fun);

  // const FunctionSpecifierContext &functionContext =
  //     specifier.GetFunctionSpecifierContext();
  // assert(functionContext.HasFunction());

  // Build the DeclName
  DeclNameInfo nameInfo;

  // Parse function name.
  auto name = GetIdentifier(curTok.GetText());
  DeclName fullName(&name);
  nameInfo.SetName(fullName);

  // very simple for now.
  SrcLoc nameLoc = ConsumeToken(tok::identifier);
  nameInfo.SetNameLoc(nameLoc);

  // FunDecl *funDecl = syntax.MakeFunDecl(&name, nameLoc, nullptr);

  // // TODO: Think about this part

  // funDecl->SetAccessLevel(specifier.GetAccessLevel());
  // funDecl->SetFunLoc(funLoc);

  // // funDecl->SetTemplate...
  // // QualType *returnType = nullptr;
  // // DeclName fullName;

  // // Scope is functin signaure
  // if (ParseFunctionSignature(specifier, *funDecl).IsError()) {
  //   return syn::MakeSyntaxError();
  // }

  // // Scope is now function body
  // if (ParseFunctionBody(specifier, *funDecl).IsError()) {
  //   return syn::MakeSyntaxError();
  // }
  // // syntax.VerifyDecl(funDecl);

  return syn::MakeSyntaxResult<Decl>(nullptr);
}

SyntaxStatus Parser::ParseFunctionSignature(ParsingDeclSpecifier &specifier,
                                            FunDecl &funDecl) {

  // ParsingScope syntaxScope(SyntaxKind::FunctionSignature);

  // TODO:
  // if(name == "Main"){
  //   sf.SetHasMainFun(true)
  // }
  // funDecl->SetDeclName(name);

  // Get Identifier
  // funDecl->SetIdentifier();

  ParseFunctionArguments(specifier, funDecl);

  SrcLoc arrowLoc;

  if (!ConsumeIf(tok::arrow, arrowLoc)) {
    // FixIt ':' to '->'.
    PrintD(curTok, diag::err_expected_arrow_after_function_param)
        .WithFix()
        .Replace(curTok.GetLoc(), llvm::StringRef("->"));

    // arrowLoc = ConsumeToken(tok::colon);
  }

  // assert(curTok.Is(tok::arrow) && "Require '->'");
  // auto arrowLoc = ConsumeToken(tok::arrow);

  // ParseReturnType();

  // Parse the return type
  // funDecl->SetReturnType();

  SyntaxResult<QualType> resultType =
      ParseDeclResultType(specifier.GetTypeSpecifierContext(),
                          diag::err_expected_type_for_function_result);

  // ConsumeToken();
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::ParseFunctionArguments(ParsingDeclSpecifier &specifier,
                                            FunDecl &funDecl) {

  assert(curTok.Is(tok::l_paren) && "Require '(' brace.");
  auto lParenLoc = ConsumeToken(tok::l_paren);

  assert(curTok.Is(tok::r_paren) && "Require ')' brace.");
  auto rParenLoc = ConsumeToken(tok::r_paren);

  // auto result = ParseDeclResultType();

  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::ParseFunctionBody(ParsingDeclSpecifier &specifier,
                                       FunctionDecl &funDecl) {

  assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  auto lParenLoc = ConsumeToken(tok::l_brace);

  assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  auto rParenLoc = ConsumeToken(tok::r_brace);

  return syn::MakeSyntaxSuccess();
}

BraceStmt *Parser::ParseFunctionBodyImpl(ParsingDeclSpecifier &specifier,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

SyntaxResult<Decl> Parser::ParseStructDecl(ParsingDeclSpecifier &specifier) {

  return syn::MakeSyntaxResult<Decl>(nullptr);
}