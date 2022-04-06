#include "stone/Compile/Parser.h"
#include "stone/Syntax/Syntax.h"
#include "stone/Syntax/SyntaxBuilder.h"
#include "stone/Syntax/SyntaxNode.h"

using namespace stone;
using namespace stone::syn;

bool Parser::AtStartOfDecl(const Token &token) {
  switch (token.GetKind()) {
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
    return true;
  default:
    return false;
  }
}
void Parser::ParseTopLevelDecls(
    llvm::SmallVector<SyntaxResult<Decl>> &results) {
  // Prime the Parser's token
  // The Lexer has the first token but the Parser's token defaults to tk::MAX
  // So, update the parser's token with the first token from the Lexer
  if (token.Is(tok::MAX)) {
    ConsumeTok();
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
  assert(AtStartOfDecl(token) && "Invalid top-declaration");
  return ParseDecl();
}

SyntaxResult<Decl> Parser::ParseDecl() {

  PairDelimiterBalancer pairDelimiterBalancer(*this);

  AccessLevel accessLevel = AccessLevel::None;
  switch (token.GetKind()) {
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

  if (token.IsAny(tok::kw_public, tok::kw_internal, tok::kw_private)) {
    ConsumeTok();
  }
  return ParseDecl(accessLevel);
}
SyntaxResult<Decl> Parser::ParseDecl(AccessLevel accessLevel) {
  SyntaxResult<Decl> declResult;

  // TODO: ParseTemplateDecl first before you move

  switch (token.GetKind()) {
  case tok::kw_forward:
    // syntaxResult = ParseForwardDecl();
    break;
  case tok::kw_fun:
    declResult = ParseFunDecl(accessLevel);
    break;
  default:
    break;
  }
  return declResult;
}

SyntaxResult<Decl> Parser::ParseFunDecl(AccessLevel accessLevel) {
  assert(token.GetKind() == tok::kw_fun &&
         "Attempting to parse a 'fun' decl with incorrect token.");

  FunDeclSyntaxBuilder funBuilder(syntax);
  auto funLoc = ConsumeTok(tok::kw_fun);
  funBuilder.WithFunKeyword(funLoc);
  funBuilder.WithAccessLevel(accessLevel);

  // Parse function name.
  Identifier name;
  SrcLoc nameLoc;

  // TODO:
  // auto funDecl = syntax.MakeFunDecl(token.GetLoc(), nullptr);
  // funDecl->SetAccessLevel(accessLevel);

  //

  // // funDecl->SetTemplate...

  // ParseFunctionSignature(funDecl);
  // ParseFunctionBody(funDecl);

  // // syntax.VerifyDecl(funDecl);

  // return funDecl;

  return syn::MakeSyntaxResult<Decl>(funBuilder.Build());
}

SyntaxStatus Parser::ParseFunctionSignature(FunDeclSyntaxBuilder &builder) {

  // TODO:
  // if(name == "Main"){
  //   sf.SetHasMainFun(true)
  // }
  // funDecl->SetDeclName(name);

  // Get Identifier
  // funDecl->SetIdentifier();

  ParseFunctionArguments(builder);

  // Parse the return type
  // funDecl->SetReturnType();

  // ConsumeTok();
}
SyntaxStatus Parser::ParseFunctionArguments(FunDeclSyntaxBuilder &builder) {
  // assert(token.Is(tok::l_brace) && "Require left brace.");
}

SyntaxStatus Parser::ParseFunctionBody(FunDeclSyntaxBuilder &builder) {
  // assert(token.Is(tok::l_brace) && "Require left brace.");
}
