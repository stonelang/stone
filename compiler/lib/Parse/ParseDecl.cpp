#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxNode.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsTopLevelDecl(const Token &curTok) {
  switch (curTok.GetKind()) {
  case tok::kw_interface:
  case tok::kw_fun:
  case tok::kw_any:
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
  assert(IsTopLevelDecl(curTok) && "Invalid top-declaration");
  return ParseDecl(DeclSyntaxParsingFlags::AllowTopLevel);
}

SyntaxResult<Decl> Parser::ParseDecl(DeclSyntaxParsingOptions flags) {

  // PairDelimiterBalancer pairDelimiterBalancer(*this);
  AccessLevel accessLevel = AccessLevel::None;
  switch (curTok.GetKind()) {
  case tok::kw_public:
    accessLevel = AccessLevel::Public;
    break;
  case tok::kw_internal:
    accessLevel = AccessLevel::Internal;
    break;
  default:
    accessLevel = AccessLevel::Private;
    break;
  }
  if (curTok.IsAny(tok::kw_public, tok::kw_internal, tok::kw_private)) {
    ConsumeToken();
  }
  // TODO:
  // ParseDeclModifierList(Attributes, StaticLoc, StaticSpelling);
  return ParseDecl(flags, accessLevel);
}

// This is your ParseDeclSpec
SyntaxResult<Decl> Parser::ParseDecl(DeclSyntaxParsingOptions flags,
                                     AccessLevel accessLevel) {
  SyntaxResult<Decl> declResult;

  // TODO: ParseTemplateDecl first before you move
  // ParsingDeclSpecifier pds(*this);

  switch (curTok.GetKind()) {

  case tok::kw_fun:
    declResult = ParseFunDecl(accessLevel);
    break;
  default:
    // if (IsBasicType(curTok.GetKind())) {
    // }
    break;
  }
  return declResult;
}

SyntaxResult<Decl> Parser::ParseFunDecl(AccessLevel accessLevel) {

  assert(curTok.Is(tok::kw_fun) &&
         "Attempting to parse a 'fun' decl with incorrect curTok.");

  auto funLoc = ConsumeToken(tok::kw_fun);
  // Parse function name.
  Identifier name = GetIdentifier(curTok.GetText());
  // very simple for now.
  SrcLoc nameLoc = ConsumeToken(tok::identifier);

  FunDecl *funDecl = syntax.MakeFunDecl(name, nameLoc, nullptr);
  funDecl->SetAccessLevel(accessLevel);
  funDecl->SetFunLoc(funLoc);

  // funDecl->SetTemplate...

  // QualType *returnType = nullptr;
  // DeclName fullName;

  // Scope is functin signaure
  if (ParseFunctionSignature(*funDecl).IsError()) {
    return syn::MakeSyntaxError();
  }

  // Scope is now function body
  if (ParseFunctionBody(*funDecl).IsError()) {
    return syn::MakeSyntaxError();
  }
  // syntax.VerifyDecl(funDecl);

  return syn::MakeSyntaxResult<Decl>(funDecl);
}

SyntaxStatus Parser::ParseFunctionSignature(FunDecl &funDecl) {

  // SyntaxParsingScope syntaxScope(SyntaxKind::FunctionSignature);

  // TODO:
  // if(name == "Main"){
  //   sf.SetHasMainFun(true)
  // }
  // funDecl->SetDeclName(name);

  // Get Identifier
  // funDecl->SetIdentifier();

  ParseFunctionArguments(funDecl);

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
      ParseDeclResultType(diag::err_expected_type_for_function_result);

  // ConsumeToken();
  return syn::MakeSyntaxSuccess();
}
SyntaxStatus Parser::ParseFunctionArguments(FunDecl &funDecl) {

  assert(curTok.Is(tok::l_paren) && "Require '(' brace.");
  auto lParenLoc = ConsumeToken(tok::l_paren);

  assert(curTok.Is(tok::r_paren) && "Require ')' brace.");
  auto rParenLoc = ConsumeToken(tok::r_paren);

  // auto result = ParseDeclResultType();

  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::ParseFunctionBody(FunctionDecl &funDecl) {

  assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  auto lParenLoc = ConsumeToken(tok::l_brace);

  assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  auto rParenLoc = ConsumeToken(tok::r_brace);

  return syn::MakeSyntaxSuccess();
}

BraceStmt *Parser::ParseFunctionBodyImpl(FunctionDecl &funDecl) {
  return nullptr;
}
