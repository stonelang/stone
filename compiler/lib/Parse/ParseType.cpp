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

  assert(collector.GetTypeSpecifierCollector().NotHasTypeSpecifierKind() &&
         "Function type-specifier comes after ->");

  if (collector.GetTypeQualifierCollector().HasAnyTypeQualifier()) {
    assert(collector.GetTypeQualifierCollector().HasPureOnly() &&
           "Function can have only 'pure' type-specifier at this point");
  }

  SyntaxStatus status;

  // Collect the type -- only basic types for now.
  status = CollectBasicTypeDecl(collector);

  if (!collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()) {
    // TODO: log that "Function requires a return type. Try '-> void' if it has
    assert(false && "No return type specified");
    // no return"
  }

  // Collect the type pattersn
  CollectTypePatterns(collector);

  // Requires at least a direct type pattern which is just the type by itself.
  assert(collector.GetTypePatternCollector().HasTypePatterns());

  // TODO: Call parseType to get the actual type

  auto qualTypeRep = new (GetSyntaxContext()) QualTypeRep();
  auto functionTypeRep = new (GetSyntaxContext()) FunctionTypeRep(qualTypeRep);

  return syn::MakeSyntaxResult<TypeRep>(functionTypeRep);
}

// Similar to ParseDeclSpecifiers
SyntaxResult<TypeRep> Parser::ParseType(ParsingDeclCollector &collector,
                                        Diag<> diagID) {

  SyntaxResult<TypeRep> result;
  ParsingScope parsingType(*this, ScopeKind::Type, "parsing type");

  return result;
}

SyntaxResult<TypeRep>
Parser::ParseDeclResultType(ParsingDeclCollector &collector, Diag<> diagID) {

  if (collector.GetFunctionSpecifierCollector().HasFun()) {
    return ParseFunctionType(collector, diagID);
  }

  return ParseType(collector, diagID);
}

SyntaxResult<QualType> Parser::ParseBasicType(TypeSpecifierCollector &collector,
                                              Diag<> diagID) {

  SyntaxResult<QualType> result;
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
