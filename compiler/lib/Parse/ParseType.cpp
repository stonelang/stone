#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/TypeRep.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

SyntaxResult<TypeRep> Parser::ParseFunctionType(ParsingDeclCollector &collector,
                                                Diag<> diagID) {

  SyntaxResult<TypeRep> result;
  ParsingScope parsingType(*this, ScopeKind::FunctionType, "parsing type");
  // TODO: We are asserting these for now but we may just want to log some ass
  // erros

  assert(collector.GetFunctionSpecifierCollector().HasFun());
  assert(collector.GetFunctionSpecifierCollector().GetArrowLoc().isValid());

  assert(collector.GetTypeSpecifierCollector().NotHasAny() &&
         "Function type-specifier comes after ->");

  if (collector.GetTypeQualifierCollector().HasAny()) {
    assert(collector.GetTypeQualifierCollector().HasPureOnly() &&
           "Function can have only 'pure' type-specifier at this point");
  }

  SyntaxStatus status;

  SyntaxResult<TypeRep> typeRep =
      ParseType(collector, diag::err_expected_type_for_function_result);

  if (!collector.GetTypeSpecifierCollector().HasAny()) {
    // TODO: log that "Function requires a return type. Try '-> void' if it has
    assert(false && "No return type specified");
    // no return"
  }

  // Collect the type pattersn
  CollectTypePatterns(collector);

  // Requires at least a direct type pattern which is just the type by itself.
  assert(collector.GetTypePatternCollector().HasAny());

  // TODO: Call parseType to get the actual type

  auto qualTypeRep = new (GetSyntaxContext()) QualifierTypeRep();
  auto functionTypeRep = new (GetSyntaxContext()) FunctionTypeRep(qualTypeRep);

  return syn::MakeSyntaxResult<TypeRep>(functionTypeRep);
}

// Similar to ParseDeclSpecifiers
SyntaxResult<TypeRep> Parser::ParseType(ParsingDeclCollector &collector,
                                        Diag<> diagID) {

  SyntaxResult<TypeRep> result;
  ParsingScope parsingType(*this, ScopeKind::Type, "parsing type");

  // if (collector.GetFunctionSpecifierCollector().HasFun() &&
  //     collector.GetFunctionSpecifierCollector().GetArrowLoc().isValid()) {
  //   // TODO:: Not too happy witht this
  //   if (GetCurScope()->GetKind() != ScopeKind::FunctionType) {
  //     return ParseFunctionType(collector, diagID);
  //   }
  // }

  result = ParseBasicType(collector, diagID);
  return result;
}

SyntaxResult<TypeRep>
Parser::ParseDeclResultType(ParsingDeclCollector &collector, Diag<> diagID) {
  return ParseType(collector, diagID);
}

SyntaxResult<TypeRep> Parser::ParseBasicType(ParsingDeclCollector &collector,
                                             Diag<> diagID) {
  SyntaxResult<TypeRep> result;
  if (!GetTok().IsBasicType()) {
    return result;
  }
  // Collect the type -- only basic types for now (TODO: user type  and function
  // types)
  auto status = CollectBasicTypeDecl(collector);

  // assert(IsBasicType(curTok.GetKind()) &&
  //        "The current token is not a basic type");

  // /// Maybe parse ParseDeclarator -- or just take a look at the code
  // switch (curTok.GetKind()) {
  // case tok::identifier: {
  //   ParseIdentifierType(collector, diagID);
  //   break;
  // }
  // default:
  //   break;
  // }
  result;
}

void Parser::ParseIdentifierType(TypeSpecifierCollector &collector,
                                 Diag<> diagID) {}
