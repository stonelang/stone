#include "stone/Basic/Defer.h"
#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"
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

  assert(IsStartOfDecl(curTok) && "Invalid top-declaration");
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

    /// Collect the specifier and it's associated quals
    status |= collector.Collect();
    if (status.HasCodeCompletion()) {
      goto EndParse;
    }

    status |= collector.IsDoubleDiping();
    if (status.IsError()) {
      goto EndParse;
    }
    /// collector.GetUsingSpecifier().HasUsings()
    if (collector.GetFunctionSpecifierCollector().HasFun()) {
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

SyntaxStatus ParsingDeclCollector::CollectUntil(tok kind) {
  SyntaxStatus status;
  while (GetParser().GetCurTok().IsNot(kind)) {
    status |= CollectImpl();
    if (status.HasCodeCompletion()) {
      break;
    }
  }
  return status;
}
SyntaxStatus ParsingDeclCollector::Collect() { return CollectImpl(); }
SyntaxStatus ParsingDeclCollector::IsDoubleDiping() {
  SyntaxStatus status;
  return status;
}

SyntaxStatus ParsingDeclCollector::CollectImpl() {
  SyntaxStatus status;
  switch (GetParser().GetCurTok().GetKind()) {
  case tok::kw_public:
    GetAccessLevelCollector().AddPublic(GetParser().ConsumeToken());
    break;
  case tok::kw_internal:
    GetAccessLevelCollector().AddInternal(GetParser().ConsumeToken());
    break;
  case tok::kw_private:
    GetAccessLevelCollector().AddPrivate(GetParser().ConsumeToken());
    break;
  case tok::kw_const:
    GetTypeQualifierCollector().AddConst(GetParser().ConsumeToken());
    break;
  case tok::kw_restrict:
    GetTypeQualifierCollector().AddRestrict(GetParser().ConsumeToken());
    break;
  case tok::kw_volatile:
    GetTypeQualifierCollector().AddVolatile(GetParser().ConsumeToken());
    break;
  case tok::kw_pure:
    GetTypeQualifierCollector().AddPure(GetParser().ConsumeToken());
    break;
  case tok::kw_static:
    GetStorageSpecifierCollector().AddStatic(GetParser().ConsumeToken());
    break;
  case tok::kw_auto:
    // TODO: Storage specifier
    GetTypeSpecifierCollector().AddAuto(GetParser().ConsumeToken());
    break;
  case tok::kw_register:
    // TODO: Storage specifier
    GetStorageSpecifierCollector().AddRegister(GetParser().ConsumeToken());
    break;
  case tok::kw_fun:
    GetFunctionSpecifierCollector().AddFun(GetParser().ConsumeToken());
    break;
  case tok::kw_inline:
    GetFunctionSpecifierCollector().AddInline(GetParser().ConsumeToken());
    break;
  case tok::kw_struct:
    GetTypeSpecifierCollector().AddStruct(GetParser().ConsumeToken());
    break;
  case tok::kw_interface:
    GetTypeSpecifierCollector().AddInterface(GetParser().ConsumeToken());
    break;
  case tok::kw_int:
    GetTypeSpecifierCollector().AddInt(GetParser().ConsumeToken());
    break;
  case tok::kw_int8:
    GetTypeSpecifierCollector().AddInt8(GetParser().ConsumeToken());
    break;
  case tok::kw_int16:
    GetTypeSpecifierCollector().AddInt16(GetParser().ConsumeToken());
    break;
  case tok::kw_int32:
    GetTypeSpecifierCollector().AddInt32(GetParser().ConsumeToken());
    break;
  case tok::kw_int64:
    GetTypeSpecifierCollector().AddInt64(GetParser().ConsumeToken());
    break;
  case tok::kw_uint:
    GetTypeSpecifierCollector().AddUInt(GetParser().ConsumeToken());
    break;
  case tok::kw_uint8:
    GetTypeSpecifierCollector().AddUInt8(GetParser().ConsumeToken());
    break;
  case tok::kw_byte:
    GetTypeSpecifierCollector().AddByte(GetParser().ConsumeToken());
    break;
  case tok::kw_uint16:
    GetTypeSpecifierCollector().AddUInt16(GetParser().ConsumeToken());
    break;
  case tok::kw_uint32:
    GetTypeSpecifierCollector().AddUInt32(GetParser().ConsumeToken());
    break;
  case tok::kw_uint64:
    GetTypeSpecifierCollector().AddUInt64(GetParser().ConsumeToken());
    break;
  case tok::kw_float:
    GetTypeSpecifierCollector().AddFloat(GetParser().ConsumeToken());
    break;
  case tok::kw_float32:
    GetTypeSpecifierCollector().AddFloat32(GetParser().ConsumeToken());
    break;
  case tok::kw_float64:
    GetTypeSpecifierCollector().AddFloat64(GetParser().ConsumeToken());
    break;
  case tok::kw_complex32:
    GetTypeSpecifierCollector().AddComplex32(GetParser().ConsumeToken());
    break;
  case tok::kw_complex64:
    GetTypeSpecifierCollector().AddComplex64(GetParser().ConsumeToken());
    break;
  default:
    // We did not find a type -- this could mean tha one was not specified or
    // it is a user defined type
    status.SetHasCodeCompletion();
    break;
  }
  return status;
}

SyntaxResult<Decl> Parser::ParseVarDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;

  ParsingScope varDeclScope(*this, ScopeKind::VarDecl,
                            "parsing var declaration");

  // NOTE: You must check the current scope to determine the DeclaratorScopeKind
  // to you
  //  ParsingDeclaratorCollector declaratorCollector(
  //      collector, DeclaratorContextKind::Variable);

  // auto status = ParseDeclarator(declaratorCollector);

  // if (!curTok.IsIdentifierOrUnderscore()) {
  // }
  return result;
}

SyntaxResult<Decl> Parser::ParseAutoDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;
  ParsingScope autoDeclScope(*this, ScopeKind::AutoDecl,
                             "parsing auto storage declaration");
  return result;
}

SyntaxStatus Parser::ParseDeclarator(ParsingDeclaratorCollector &collector) {
  SyntaxStatus status;

  assert(collector.GetParsingDeclCollector()
             .GetTypeSpecifierCollector()
             .HasTypeSpecifierKind() &&
         "Attempting to parse a declarator without a type-specifier.");

  // 1. Are you parsing a VarDecl
  // if(GetCurScope().GetParent().GetKind() == ScopeKind::VarDecl){

  // }

  // if (curTok.IsPointerOperator()) {
  //   // collector.GetTypeSpecifierCollector().Bits.IsPointer = true;
  //   auto pointerDeclaratorChunk = PointerDeclaratorChunk::Create();
  //   pointerDeclaratorChunk.AddPointer();
  //   collector.AddDeclaratorChunk(pointerDeclaratorChunk, ConsumeToken());
  //   while (curTok.IsPointerOperator()) {
  //     pointerDeclaratorChunk.AddPointer();
  //     ConsumeToken();
  //   }
  // }
  // if (curTok.IsReferenceOperator()) {
  //   auto refernceDeclaratorChunk = ReferenceDeclaratorChunk::Create();
  //    refernceDeclaratorChunk.AddReference();
  //   collector.AddDeclaratorChunk(refernceDeclaratorChunk, ConsumeToken());
  //   ConsumeToken();
  //   while (curTok.IsReferenceOperator()) {
  //     refernceDeclaratorChunk.AddReference();
  //     ConsumeToken();
  //   }
  // }
  return status;
}

SyntaxResult<Decl> Parser::ParseFunDecl(ParsingDeclCollector &collector) {

  SyntaxResult<Decl> result;
  ParsingScope funDeclScope(*this, ScopeKind::FunDecl,
                            "parsing fun declaration");

  assert(collector.GetFunctionSpecifierCollector().HasFun() &&
         "Attempting to parse a function without a functin definition.");

  auto funLoc = collector.GetFunctionSpecifierCollector().GetFunLoc();

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
    // We only allow pure on functions => non-member function
    if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
      // Do some logging
      return result;
    }
  }

  if (collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()) {
    // TODO: Log a message -- not allowed to have type specs here
    return result;
  }

  // ParsingDeclaratorCollector declaratorCollector(collector);
  // auto status = ParseDeclarator(declaratorCollector);

  if (!GetCurTok().IsIdentifierOrUnderscore()) {
    // Do some logging  "Expecting function declarator or identifier");
    return result;
  }

  // ParsingDeclarator parsingDeclarator(collector);
  // ParseDeclarator(parsingDeclarator);
  // ParseDeclName();

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
    collector.GetFunctionSpecifierCollector().AddIsMember();
  }

  if (!collector.GetFunctionSpecifierCollector().HasIsMember() &&
      collector.GetStorageSpecifierCollector().HasStatic()) {
    // Log only member functions can be status
    return result;
  }
  SyntaxStatus status;
  // Now, parse the function signature
  status |= ParseFunctionSignature(nameInfo, collector);

  if (status.IsError()) {
    return result;
  }

  // Create the function
  auto funDecl =
      FunDeclFactory::Create(nameInfo, sc, nullptr, GetCurDeclContext());
  assert(funDecl);

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
  return result;
}

SyntaxStatus Parser::ParseFunctionSignature(const DeclNameInfo &nameInfo,
                                            ParsingDeclCollector &collector) {
  SyntaxStatus status;
  ParsingScope funSigScope(*this, ScopeKind::FunctionSignature,
                           "parsing fun signature");

  status |= ParseFunctionArguments(collector);
  if (status.IsError()) {
    return status;
  }

  SrcLoc arrowLoc;
  if (curTok.IsArrow()) {
    ParsingScope functionResult(*this, ScopeKind::ReturnClause,
                                "parsing result");
    if (!ConsumeIf(tok::arrow, arrowLoc)) {
      // FixIt ':' to '->'.
      PrintD(curTok, diag::err_expected_arrow_after_function_param)
          .WithFix()
          .Replace(curTok.GetLoc(), llvm::StringRef("->"));
      // arrowLoc = ConsumeToken(tok::colon);
    }
    if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
      if (!collector.GetTypeQualifierCollector().HasPureOnly()) {
        // TODO: Log
        status.SetIsError();
        return status;
      }
    }
    // We can call collect here to get the return type
    collector.CollectUntil(tok::l_brace);
    if (!collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()) {
      // Perform some logging function must return a function type
      status.SetIsError();
      return status;
    }
    // TODO: Look for TypeSpecs
    // SyntaxResult<QualType> resultType = ParseDeclResultType(
    //     collector, diag::err_expected_type_for_function_result);
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

SyntaxStatus Parser::ParseFunctionBody(ParsingDeclCollector &collector,
                                       FunctionDecl &funDecl) {

  SyntaxStatus status;
  ParsingScope funBodyScope(*this, ScopeKind::FunctionBody,
                            "parsing fun arguments");

  assert(curTok.Is(tok::l_brace) && "Require '{' brace.");
  auto lParenLoc = ConsumeToken(tok::l_brace);

  assert(curTok.Is(tok::r_brace) && "Require '}' brace.");
  auto rParenLoc = ConsumeToken(tok::r_brace);

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

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
    return result;
  }
  return result;
}

void ParsingDeclCollector::Verify() {}

void ParsingDeclCollector::Apply() {}
