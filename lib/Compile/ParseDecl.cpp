#include "stone/Compile/Parser.h"
#include "stone/Compile/ParserRAII.h"
#include "stone/Utils/Ret.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

bool Parser::IsTopDeclStart(const Token &tok) {
  switch (tok.GetType()) {
  case tk::Type::kw_interface:
  case tk::Type::kw_fun:
  case tk::Type::kw_any:
  case tk::Type::kw_struct:
  case tk::Type::kw_space:
  case tk::Type::kw_const:
  case tk::Type::kw_public:
  case tk::Type::kw_private:
  case tk::Type::kw_internal:
  case tk::Type::kw_static:
    return true;
  default:
    return false;
  }
}

// Ex: sample.stone
// fun F0() -> void {}
// fun F1() -> void {}
// There are two top decls - F0 and F1
// This call parses one at a time and adds it to the SyntaxFile
bool Parser::ParseTopDecl(syn::DeclGroupPtrTy &result, bool isFirstDecl) {

  assert(IsTopDeclStart(tok) && "Invalid top-declaration");

  ParseDecl();

  return true;
}

static bool HasAccessLevel(const syn::Token &tok) {
  switch (tok.GetType()) {
  case tk::Type::kw_public:
  case tk::Type::kw_internal:
  case tk::Type::kw_private:
    return true;
  default:
    return false;
  }
}
syn::DeclGroupPtrTy Parser::ParseDecl(ParsingDeclSpecifier *pds) {

  PairDelimiterBalancer pairDelimiterBalancer(*this);

  AccessLevel accessLevel = AccessLevel::None;
  switch (tok.GetType()) {
  case tk::Type::kw_public:
    accessLevel = AccessLevel::Public;
    break;
  case tk::Type::kw_internal:
    accessLevel = AccessLevel::Internal;
    break;
  default:
    accessLevel = AccessLevel::Private;
    break;
  }
  if (HasAccessLevel(tok)) {
    ConsumeTok();
  }
  if (pds) {
    return ParseDecl(*pds, accessLevel);
  }

  ParsingDeclSpecifier localPDS(*this);
  return ParseDecl(localPDS, accessLevel);
}
syn::DeclGroupPtrTy Parser::ParseDecl(ParsingDeclSpecifier &pds,
                                      AccessLevel accessLevel) {

  SyntaxResult<Decl *> syntaxResult;

  // TODO: ParseTemplateDecl first before you move

  switch (tok.GetType()) {
  case tk::Type::kw_any:
    // ParseTemplateDecl();
    break;
  case tk::Type::kw_fun:
    syntaxResult = ParseFunDecl(pds, accessLevel);
    break;
  default:
    break;
  }
  // return CreateDeclGroup(singleDecl);
  return nullptr;
}

void Parser::ParseFunctionSignature(FunDecl *funDecl) {

  assert(funDecl && "Null FunDecl");
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
  // assert(tok.Is(tk::Type::l_brace) && "Require left brace.");
}
void Parser::ParseFunctionArguments(FunDecl *funDecl) {

  assert(funDecl && "Null FunDecl");
  // assert(tok.Is(tk::Type::l_brace) && "Require left brace.");
}
SyntaxResult<Decl *> Parser::ParseFunDecl(ParsingDeclSpecifier &pds,
                                          AccessLevel accessLevel) {

  assert(tok.GetType() == tk::Type::kw_fun &&
         "Attempting to parse a 'fun' decl with incorrect token.");

  // TODO:
  auto funDecl = syntax.MakeFunDecl(tok.GetLoc(), nullptr);
  funDecl->SetAccessLevel(accessLevel);

  ConsumeTok(tk::Type::kw_fun);

  // funDecl->SetTemplate...

  ParseFunctionSignature(funDecl);
  ParseFunctionBody(funDecl);

  // syntax.VerifyDecl(funDecl);

  if (check) {
  }

  return funDecl;
}
