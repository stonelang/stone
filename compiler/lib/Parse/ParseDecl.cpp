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
  assert(IsStartOfDecl(curTok) && "Invalid start of or top level declaration");

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
  ParsingContext parsingContext(ParsingContextKind::Decl);

  while (true) {
  BeginParse:

    if (IsDone() || curTok.IsIdentifierOrUnderscore()) {
      goto EndParse;
    }

    if (!spec.HasAccessLevel()) {
      status |= ParseAccessLevel(spec);
      if (status.hasCodeCompletion()) {
        goto BeginParse;
      }
    }
    if (!spec.GetTypeQualifireContext().HasTypeQualifiers()) {
      /// Look for any type qualifiers: const, volatile, restrict, etc.
      status |= ParseTypeQualifiers(spec.GetTypeQualifireContext());
      if (status.hasCodeCompletion()) {
        goto BeginParse;
      }
    }

    if (!spec.GetTypeSpecifierContext().IsBasicType()) {
      status |= ParseBasicTypeSpecifier(spec.GetTypeSpecifierContext());
      if (status.hasCodeCompletion()) {
        goto BeginParse;
      }
    }

    if (!spec.GetFunctionSpecifierContext().HasFun()) {
      if (curTok.IsFun()) {
        spec.GetFunctionSpecifierContext().AddFun(ConsumeToken());
        // This goes into the ParseFunDecl
        // if (PeekNextToken().IsDoubleColon()) {
        // specifier.GetFunctionSpecifierContext().SetIsMember();
        //}
        result = ParseFunDecl(spec);
      }
      goto EndParse;
    }

    if (!spec.GetTypeSpecifierContext().IsStruct()) {
      if (curTok.IsStruct()) {
        spec.GetTypeSpecifierContext().AddStruct(ConsumeToken());
        result = ParseStructDecl(spec);
      }
      goto EndParse;
    }

    if (!spec.GetTypeSpecifierContext().IsInterface()) {
      if (curTok.IsInterface()) {
        spec.GetTypeSpecifierContext().AddInterface(ConsumeToken());
        result = ParseInterfaceDecl(spec);
      }
      goto EndParse;
    }

    if (!spec.GetTypeSpecifierContext().IsEnum()) {
      if (curTok.IsEnum()) {
        spec.GetTypeSpecifierContext().AddEnum(ConsumeToken());
        // result = ParseEnumDecl(spec);
      }
      goto EndParse;
    }

  } // End of while

EndParse : { return result; }

  // ParseDeclSpecifier(spec);

  // if (spec.GetFunctionSpecifierContext().HasFun()) {
  //   return (result = ParseFunDecl(spec));
  // }

  // switch (spec.GetTypeSpecifierContext().GetTypeSpecifierKind()) {
  // case TypeSpecifierKind::Enum:
  //   return (result = ParseEnumDecl(spec));
  // case TypeSpecifierKind::Struct:
  //   return (result = ParseStructDecl(spec));
  // case TypeSpecifierKind::Interface:
  //   return (result = ParseInterfaceDecl(spec));
  // default:
  //   break;
  // }

  // if (spec.GetTypeSpecifierContext().IsBasicType() ||
  //     spec.GetTypeSpecifierContext().IsAuto()) {

  //   if (!curTok.IsIdentifierOrUnderscore()) {
  //     // Error -- type declaration is missing a declarator/identifier
  //     return result;
  //   } else {
  //     ParsingDeclarator declarator(spec,
  //     DeclaratorContextKind::SyntaxFile); return (result =
  //     ParseVarDecl(declarator));
  //   }
  // }

  // // Ok, it sesms that we have a random identifier
  // if (curTok.IsIdentifierOrUnderscore()) {
  //   // Error -- declarator/identifier is missing a type
  // }
  return result;
}

void Parser::ParseDeclSpecifier(ParsingDeclSpecifier &spec) {

  // The ordering does not matter becuase the compiler will eventually
  // order things in a nice way -- we are building up the DecInfo
  while (true) {

    if (IsDone() || curTok.IsIdentifierOrUnderscore()) {
      return;
    }
    switch (curTok.GetKind()) {
    // Access levels
    case tok::kw_public:
      spec.AddPublicAccessLevel(ConsumeToken());
      break;
    case tok::kw_internal:
      spec.AddInternalAccessLevel(ConsumeToken());
      break;
    case tok::kw_private:
      spec.AddInternalAccessLevel(ConsumeToken());
      break;

      /// CRV
    case tok::kw_const:
      spec.GetTypeQualifireContext().AddConst(ConsumeToken());
      // We do not consume the token because the QualType that we create
      // will be of the following const int i = ....
      break;
    case tok::kw_restrict:
      spec.GetTypeQualifireContext().AddRestrict(ConsumeToken());
      break;
    case tok::kw_volatile:
      spec.GetTypeQualifireContext().AddVolatile(ConsumeToken());
      break;

    // Functions
    case tok::kw_inline:
      spec.GetFunctionSpecifierContext().AddInline(ConsumeToken());
      break;
    case tok::kw_fun:
      spec.GetFunctionSpecifierContext().AddFun(ConsumeToken());
      goto EndParse;

      // Nominals
    case tok::kw_struct:
      spec.GetTypeSpecifierContext().AddStruct(ConsumeToken());
      goto EndParse;
    case tok::kw_enum:
      spec.GetTypeSpecifierContext().AddEnum(ConsumeToken());
      goto EndParse;
    case tok::kw_interface:
      spec.GetTypeSpecifierContext().AddInterface(ConsumeToken());
      goto EndParse;
    case tok::kw_auto:
      spec.GetTypeSpecifierContext().AddAuto(ConsumeToken());
      goto EndParse;

      // Basic types
    case tok::kw_int:
      spec.GetTypeSpecifierContext().AddInt(ConsumeToken());
      goto EndParse;
    case tok::kw_int8:
      spec.GetTypeSpecifierContext().AddInt8(ConsumeToken());
      goto EndParse;
    case tok::kw_int16:
      spec.GetTypeSpecifierContext().AddInt16(ConsumeToken());
      goto EndParse;
    case tok::kw_int32:
      spec.GetTypeSpecifierContext().AddInt32(ConsumeToken());
      goto EndParse;
    case tok::kw_int64:
      spec.GetTypeSpecifierContext().AddInt64(ConsumeToken());
      goto EndParse;
    case tok::kw_uint:
      spec.GetTypeSpecifierContext().AddUInt(ConsumeToken());
      goto EndParse;
    case tok::kw_uint8:
      spec.GetTypeSpecifierContext().AddUInt8(ConsumeToken());
      goto EndParse;
    case tok::kw_byte:
      spec.GetTypeSpecifierContext().AddByte(ConsumeToken());
      goto EndParse;
    case tok::kw_uint16:
      spec.GetTypeSpecifierContext().AddUInt16(ConsumeToken());
      goto EndParse;
    case tok::kw_uint32:
      spec.GetTypeSpecifierContext().AddUInt32(ConsumeToken());
      goto EndParse;
    case tok::kw_uint64:
      spec.GetTypeSpecifierContext().AddUInt64(ConsumeToken());
      goto EndParse;
    case tok::kw_float:
      spec.GetTypeSpecifierContext().AddFloat(ConsumeToken());
      goto EndParse;
    case tok::kw_float32:
      spec.GetTypeSpecifierContext().AddFloat32(ConsumeToken());
      goto EndParse;
    case tok::kw_float64:
      spec.GetTypeSpecifierContext().AddFloat64(ConsumeToken());
      goto EndParse;
    case tok::kw_complex32:
      spec.GetTypeSpecifierContext().AddComplex32(ConsumeToken());
      goto EndParse;
    case tok::kw_complex64:
      spec.GetTypeSpecifierContext().AddComplex64(ConsumeToken());
      goto EndParse;
    // Identifier marks the end
    case tok::identifier:
      goto EndParse;
    default: {

      /// TODO: Figure out why we are here -- this must be an error
      break;
    }
    }
  }
EndParse : { return; }
}

SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDeclarator &declarator) {

  // assert(spec.GetTypeSpecifierContext().HasTypeSpecifierKind());
  // const ParsingDeclSpecifier &spec =
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

SyntaxResult<Decl> Parser::ParseFunDecl(ParsingDeclSpecifier &spec) {

  SyntaxStatus status;

  assert(spec.GetFunctionSpecifierContext().HasFun() &&
         "Attempting to parse a function without a functin definition.");

  auto funLoc = spec.GetFunctionSpecifierContext().GetFunLoc();

  // At this point, we are expecting an identifier
  assert(curTok.IsIdentifierOrUnderscore() &&
         "Expecting function declarator or identifier");

  // Build the DeclName
  DeclNameInfo nameInfo;

  // Parse function name.
  auto name = GetIdentifier(curTok.GetText());
  DeclName fullName(&name);
  nameInfo.SetName(fullName);

  // very simple for now.
  SrcLoc nameLoc = ConsumeToken(tok::identifier);
  nameInfo.SetNameLoc(nameLoc);

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
  return status;
}

SyntaxStatus Parser::ParseFunctionSignature(const DeclNameInfo &nameInfo,
                                            ParsingDeclSpecifier &spec) {

  SyntaxStatus status;
  // ParsingScope syntaxScope(SyntaxKind::FunctionSignature);

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
  SyntaxStatus status;

  if (!curTok.IsLeftParen()) {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
  } else {
    // Just consume for now
    auto lParenLoc = ConsumeToken(tok::l_paren);
  }

  if (!curTok.IsRightParen()) {
    // If we don't have the leading '(', complain.
    // auto diag = PrintD(Tok, diagID);
  } else {
    // Just consume for now
    auto lParenLoc = ConsumeToken(tok::r_paren);
  }
  // auto result = ParseDeclResultType();

  return status;
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
