#include "stone/Compile/Parser.h"
#include "stone/Syntax/Syntax.h"
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
    llvm::SmallVector<SyntaxResult<Decl *>> &results) {
  // Prime the Parser's token
  // The Lexer has the first token but the Parser's token defaults to tk::MAX
  // So, update the parser's token with the first token from the Lexer
  if (token.Is(tok::MAX)) {
    ConsumeTok();
  }
  SyntaxResult<Decl *> result;
  while (!IsDone()) {
    ParseTopLevelDecl(result);
    if (HasError()) {
      if (listener) {
        listener->OnError();
      }
      return;
    }
    results.push_back(result.Get());
  }
}
// Ex: sample.stone
// fun F0() -> void {}
// fun F1() -> void {}
// There are two top decls - F0 and F1
// This call parses one at a time and adds it to the SyntaxFile
void Parser::ParseTopLevelDecl(SyntaxResult<Decl *> &result) {
  assert(AtStartOfDecl(token) && "Invalid top-declaration");
}

static bool HasAccessLevel(const syn::Token &token) {
  switch (token.GetKind()) {
  case tok::kw_public:
  case tok::kw_internal:
  case tok::kw_private:
    return true;
  default:
    return false;
  }
}
SyntaxResult<Decl *> Parser::ParseDecl(ParsingDeclSpecifier *pds) {
  PairDelimiterBalancer pairDelimiterBalancer(*this);

  // We always default to private
  AccessLevel accessLevel = AccessLevel::Private;

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
  if (HasAccessLevel(token)) {
    ConsumeTok();
  }
  if (pds) {
    return ParseDecl(*pds, accessLevel);
  }

  ParsingDeclSpecifier localPDS(*this);
  return ParseDecl(localPDS, accessLevel);
}
SyntaxResult<Decl *> Parser::ParseDecl(ParsingDeclSpecifier &pds,
                                       AccessLevel accessLevel) {
  SyntaxResult<Decl *> syntaxResult;

  // TODO: ParseTemplateDecl first before you move

  switch (token.GetKind()) {
  case tok::kw_any:
    // ParseTemplateDecl();
    break;
  case tok::kw_fun:
    syntaxResult = ParseFunDecl(pds, accessLevel);
    break;
  default:
    break;
  }
  // return CreateDeclGroup(singleDecl);
  return DeclResult();
}

void Parser::ParseFunctionSignature(FunDecl *funDecl) {
  assert(funDecl && "Null FunDecl");

  // TODO:
  // if(name == "Main"){
  //   sf.SetHasMainFun(true)
  // }
  // funDecl->SetDeclName(name);

  // Get Identifier
  // funDecl->SetIdentifier();

  ParseFunctionArguments(funDecl);

  // Parse the return type
  // funDecl->SetReturnType();

  // ConsumeTok();
}

void Parser::ParseFunctionBody(FunDecl *funDecl) {
  assert(funDecl && "Null FunDecl");
  // assert(token.Is(tok::l_brace) && "Require left brace.");
}
void Parser::ParseFunctionArguments(FunDecl *funDecl) {
  assert(funDecl && "Null FunDecl");
  // assert(token.Is(tok::l_brace) && "Require left brace.");
}
SyntaxResult<Decl *> Parser::ParseFunDecl(ParsingDeclSpecifier &pds,
                                          AccessLevel accessLevel) {
  assert(token.GetKind() == tok::kw_fun &&
         "Attempting to parse a 'fun' decl with incorrect token.");

  // TODO:
  auto funDecl = syntax.MakeFunDecl(token.GetLoc(), nullptr);
  funDecl->SetAccessLevel(accessLevel);

  ConsumeTok(tok::kw_fun);

  // funDecl->SetTemplate...

  ParseFunctionSignature(funDecl);
  ParseFunctionBody(funDecl);

  // syntax.VerifyDecl(funDecl);

  return funDecl;
}
