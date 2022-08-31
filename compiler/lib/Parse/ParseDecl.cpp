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

  return ParseDecl(SyntaxParsingDeclFlags::AllowTopLevel,
                   [&](Decl *d) { /* Do nothing for now*/ });
}

// NOTE: This is ripe for recursion.
SyntaxResult<Decl> Parser::ParseDecl(SyntaxParsingDeclOptions flags,
                                     llvm::function_ref<void(Decl *)> handler) {

  SyntaxParsingDeclSpecifier specifier(*this, GetAttributeFactory());
  specifier.flags = flags;

  return ParseDecl(specifier, handler);
}

/// Parse declaration specifiers
SyntaxResult<Decl> Parser::ParseDecl(SyntaxParsingDeclSpecifier &specifier,
                                     llvm::function_ref<void(Decl *)> handler) {

  SyntaxResult<Decl> declResult;
  SyntaxParsingContext parsingContext(SyntaxParsingContextKind::Decl);
  parsingContext.SetParsing();

  while (parsingContext.IsParsing() && !IsDone()) {
  Continue:
    SrcLoc loc = curTok.GetLoc();
    switch (curTok.GetKind()) {
    case tok::kw_public:
      specifier.level = AccessLevel::Public;
      ConsumeToken();
      continue;
    case tok::kw_internal:
      specifier.level = AccessLevel::Internal;
      ConsumeToken();
      continue;
    case tok::kw_private:
      specifier.level = AccessLevel::Private;
      ConsumeToken();
      goto Continue;

      continue;
    case tok::kw_fun:
      declResult = ParseFunDecl(specifier);
      parsingContext.SetDone();
      break;
    case tok::kw_struct:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Struct, loc);
      declResult = ParseStructDecl(specifier);
      parsingContext.SetDone();
      break;
    case tok::kw_const:
      specifier.GetTypeSpecifierContext().SetTypeQualifierKind(
          TypeQualifierKind::Const, loc);
      ConsumeToken();
      continue;
    case tok::kw_volatile:
      specifier.GetTypeSpecifierContext().SetTypeQualifierKind(
          TypeQualifierKind::Volatile, loc);
      ConsumeToken();
      continue;
    case tok::kw_restrict:
      specifier.GetTypeSpecifierContext().SetTypeQualifierKind(
          TypeQualifierKind::Restrict, loc);
      ConsumeToken();
      continue;
    case tok::identifier:
      if (specifier.GetTypeSpecifierContext().HasTypeSpecifierKind()) {
        SyntaxParsingDeclarator declarator(specifier,
                                           DeclaratorContextKind::SyntaxFile);
        declResult = ParseVarDecl(declarator);
      } else {
        // This is just some random variable
      }
      break;
    case tok::kw_auto:
      if (!specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
              TypeSpecifierKind::Auto, loc)) {
      }
      break;
    case tok::kw_int8:
      if (!specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
              TypeSpecifierKind::Int8, loc)) {
      }
      break;
    case tok::kw_int16:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Int16, loc);
      break;
    case tok::kw_int32:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Int32, loc);
      break;
    case tok::kw_int64:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Int64, loc);
      break;
    case tok::kw_int:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Int, loc);
      break;
    case tok::kw_uint:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::UInt, loc);
      break;
    case tok::kw_uint8:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::UInt8, loc);
      break;
    case tok::kw_byte:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Byte, loc);
      break;
    case tok::kw_uint16:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::UInt16, loc);
      break;
    case tok::kw_uint32:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::UInt32, loc);
      break;
    case tok::kw_uint64:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::UInt64, loc);
      break;
    case tok::kw_float:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Float, loc);
    case tok::kw_float32:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Float32, loc);
      break;
    case tok::kw_float64:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Float64, loc);
      break;
    case tok::kw_complex32:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Complex32, loc);
      break;
    case tok::kw_complex64:
      specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
          TypeSpecifierKind::Complex64, loc);
      break;

    default:
      break;
    }
  }
  return declResult;
}

SyntaxResult<Decl> Parser::ParseVarDecl(SyntaxParsingDeclarator &declarator) {

  // const SyntaxParsingDeclSpecifier &specifier =
  //     declarator.GetSyntaxParsingDeclSpecifier();
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
// void Parser::ParseBasicTypeSpecifier(TypeSpecifierContext &specifierContext,
// SrcLoc loc) {

//   switch (curTok.GetKind()) {
//     case tok::kw_int8:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Int8, loc);
//       break;
//     case tok::kw_int16:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Int16, loc);
//       break;
//     case tok::kw_int32:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Int32, loc);
//       break;
//     case tok::kw_int64:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Int64, loc);
//       break;
//     case tok::kw_int:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Int, loc);
//       break;
//     case tok::kw_uint:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::UInt, loc);
//       break;
//     case tok::kw_uint8:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::UInt8, loc);
//       break;
//     case tok::kw_byte:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Byte, loc);
//       break;
//     case tok::kw_uint16:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::UInt16, loc);
//       break;
//     case tok::kw_uint32:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::UInt32, loc);
//       break;
//     case tok::kw_uint64:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::UInt64, loc);
//       break;
//     case tok::kw_float:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Float, loc);
//     case tok::kw_float32:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Float32, loc);
//       break;
//     case tok::kw_float64:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Float64, loc);
//       break;
//     case tok::kw_complex32:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Complex32, loc);
//       break;
//     case tok::kw_complex64:
//       specifier.GetTypeSpecifierContext().SetTypeSpecifierKind(
//           TypeSpecifierKind::Complex64, loc);
//       break;
//     default:
//       break;
//     }
// }
SyntaxResult<Decl> Parser::ParseFunDecl(SyntaxParsingDeclSpecifier &specifier) {

  assert(curTok.Is(tok::kw_fun) &&
         "Attempting to parse a 'fun' decl with incorrect curTok.");

  auto funLoc = ConsumeToken(tok::kw_fun);
  // Parse function name.
  Identifier name = GetIdentifier(curTok.GetText());
  // very simple for now.
  SrcLoc nameLoc = ConsumeToken(tok::identifier);

  FunDecl *funDecl = syntax.MakeFunDecl(name, nameLoc, nullptr);
  funDecl->SetAccessLevel(specifier.level);
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

SyntaxStatus
Parser::ParseFunctionSignature(SyntaxParsingDeclSpecifier &specifier,
                               FunDecl &funDecl) {

  // SyntaxParsingScope syntaxScope(SyntaxKind::FunctionSignature);

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
SyntaxStatus
Parser::ParseFunctionArguments(SyntaxParsingDeclSpecifier &specifier,
                               FunDecl &funDecl) {

  assert(curTok.Is(tok::l_paren) && "Require '(' brace.");
  auto lParenLoc = ConsumeToken(tok::l_paren);

  assert(curTok.Is(tok::r_paren) && "Require ')' brace.");
  auto rParenLoc = ConsumeToken(tok::r_paren);

  // auto result = ParseDeclResultType();

  return syn::MakeSyntaxSuccess();
}

SyntaxStatus Parser::ParseFunctionBody(SyntaxParsingDeclSpecifier &specifier,
                                       FunctionDecl &funDecl) {

  assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  auto lParenLoc = ConsumeToken(tok::l_brace);

  assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  auto rParenLoc = ConsumeToken(tok::r_brace);

  return syn::MakeSyntaxSuccess();
}

BraceStmt *Parser::ParseFunctionBodyImpl(SyntaxParsingDeclSpecifier &specifier,
                                         FunctionDecl &funDecl) {
  return nullptr;
}

SyntaxResult<Decl>
Parser::ParseStructDecl(SyntaxParsingDeclSpecifier &specifier) {

  return syn::MakeSyntaxResult<Decl>(nullptr);
}