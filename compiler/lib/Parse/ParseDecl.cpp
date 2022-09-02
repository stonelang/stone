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

  while (result.IsNull() && !IsDone()) {

    if (ParseAccessLevel(specifier)) {
      ConsumeToken();
      continue;
    } else {
      // specifier.GetAccessLevelContext().AddPrivate();
    }

    // SrcLoc loc = curTok.GetLoc();
    // switch (curTok.GetKind()) {
    // case tok::kw_public:
    //   specifier.AddPublicAccessLevel(loc);
    //   ConsumeToken();
    //   continue;
    // case tok::kw_internal:
    //   specifier.AddInternalAccessLevel(loc);
    //   ConsumeToken();
    //   continue;
    // case tok::kw_private:
    //   specifier.AddPrivateAccessLevel(loc);
    //   ConsumeToken();
    //   continue;
    // case tok::kw_fun:
    //   specifier.GetFunctionSpecifierContext().AddFunctionDef(loc);
    //   result = ParseFunDecl(specifier);
    //   break;
    // case tok::kw_struct:
    //   specifier.GetTypeSpecifierContext().AddStruct(loc);
    //   result = ParseStructDecl(specifier);
    //   break;
    // case tok::kw_inline:
    //   specifier.GetFunctionSpecifierContext().AddInline(loc);
    //   break;
    // case tok::identifier:
    //   if (specifier.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
    //     ParsingDeclarator declarator(specifier,
    //                                  DeclaratorContextKind::SyntaxFile);
    //     result = ParseVarDecl(declarator);
    //   } else {
    //     // This is just some random variable with no type -- error message.
    //   }
    //   break;
    // default:
    //   if (ParseTypeQualifires(specifier.GetTypeQualifireContext())) {
    //     break;
    //   }
    //   if (ParseBasicTypeSpecifier(specifier.GetTypeSpecifierContext())) {
    //     break;
    //   }
    //   if(ParseAccessLevel(specifier)){
    //     break;
    //   }
    // } // End of switch

    // ConsumeToken();
  } // End of while
  return result;
}

SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDeclarator &declarator) {

  // const ParsingDeclSpecifier &specifier =
  //     declarator.GetParsingDeclSpecifier();
  // assert(specifier.GetTypeSpecifierContext().HasTypeSpecifierKind());
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

  assert(curTok.Is(tok::kw_fun) &&
         "Attempting to parse a 'fun' decl with incorrect curTok.");
  auto funLoc = ConsumeToken(tok::kw_fun);

  // const FunctionSpecifierContext &functionContext =
  //     specifier.GetFunctionSpecifierContext();
  // assert(functionContext.HasFunction());

  // Parse function name.
  Identifier name = GetIdentifier(curTok.GetText());
  // very simple for now.
  SrcLoc nameLoc = ConsumeToken(tok::identifier);

  FunDecl *funDecl = syntax.MakeFunDecl(&name, nameLoc, nullptr);

  // TODO: Think about this part

  funDecl->SetAccessLevel(specifier.GetAccessLevel());
  funDecl->SetFunLoc(funLoc);

  // funDecl->SetTemplate...
  // QualType *returnType = nullptr;
  // DeclName fullName;

  // Scope is functin signaure
  if (ParseFunctionSignature(specifier, *funDecl).IsError()) {
    return syn::MakeSyntaxError();
  }

  // Scope is now function body
  if (ParseFunctionBody(specifier, *funDecl).IsError()) {
    return syn::MakeSyntaxError();
  }
  // syntax.VerifyDecl(funDecl);

  return syn::MakeSyntaxResult<Decl>(funDecl);
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