#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Types.h"
#include "stone/Syntax/TypeRep.h"


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
  // Get all of the Type information
  while (!GetTok().IsLBrace() && !IsEOF()) {
    // TODO: We obviously have to check for duplicates and errors -- ok for now
    // to get this to work.
    status = CollectTypeQualifier(collector);
    status = CollectBasicTypeDecl(collector);
    status = CollectTypePatterns(collector);
  }

  // Let us see what we have

  if (!collector.GetTypeSpecifierCollector().HasTypeSpecifierKind()) {
    // TODO: log that "Function requires a return type. Try '-> void' if it has
    // no return"
  }

  // Requires at least a direct type pattern which is just the type by itself.
  assert(collector.GetTypePatternCollector().HasTypePatterns());

  // Create the FunctionTypeRep 

  // Passing full for now 
  //typeRep = CreateParseFunctionTypeRep(GetSyntaxContext());

  auto typeRep = new (GetSyntaxContext()) FunctionTypeRep(nullptr);
  return syn::MakeSyntaxResult<TypeRep>(typeRep);

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
