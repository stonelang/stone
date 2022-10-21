#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

// fun Do() -> '() -> int'
SyntaxResult<TypeBase>
Parser::ParseFunctionType(ParsingDeclCollector &collector, Diag<> diagID) {

  SyntaxResult<TypeBase> result;
  ParsingScope parsingType(*this, ScopeKind::FunctionType, "parsing type");
  // TODO: We are asserting these for now but we may just want to log some ass
  // erros

  // assert(collector.GetFunctionSpecifierCollector().HasFun());
  // assert(collector.GetFunctionSpecifierCollector().GetArrowLoc().isValid());

  // assert(collector.GetTypeSpecifierCollector().NotHasAny() &&
  //        "Function type-specifier comes after ->");

  // if (collector.GetTypeQualifierCollector().HasAny()) {
  //   assert(collector.GetTypeQualifierCollector().HasPureOnly() &&
  //          "Function can have only 'pure' type-specifier at this point");
  // }

  // SyntaxStatus status;

  // SyntaxResult<Type> type =
  //     ParseType(collector, diag::err_expected_type_for_function_result);

  // if (!collector.GetTypeSpecifierCollector().HasAny()) {
  //   // TODO: log that "Function requires a return type. Try '-> void' if it
  //   has assert(false && "No return type specified");
  //   // no return"
  // }

  // // Collect the type pattersn
  // CollectTypeChunks(collector);

  // // Requires at least a direct type pattern which is just the type by
  // itself. assert(collector.GetTypeChunkCollector().HasAny());

  // TODO: Call parseType to get the actual type

  // auto qualType = new (GetSyntaxContext()) QualifierType();
  // auto functionType = new (GetSyntaxContext()) FunctionType(qualType);

  return syn::MakeSyntaxResult<TypeBase>(nullptr);
}

// Similar to ParseDeclSpecifiers
SyntaxResult<TypeBase> Parser::ParseType(ParsingDeclCollector &collector,
                                         Diag<> diagID) {
  SyntaxResult<TypeBase> result;
  ParsingScope parsingType(*this, ScopeKind::Type, "parsing type");

  // if (collector.GetFunctionSpecifierCollector().HasFun() &&
  //     collector.GetFunctionSpecifierCollector().GetArrowLoc().isValid()) {
  //   // TODO:: Not too happy witht this
  //   if (GetCurScope()->GetKind() != ScopeKind::FunctionType) {
  //     return ParseFunctionType(collector, diagID);
  //   }
  // }

  if (GetTok().IsBasicType()) {
    result = ParseBasicType(collector, diagID);
  }

  // if (collector.GetTypeQualifierCollector().HasAny()) {
  //   QualType qualType(type);
  // }
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

  return result;
}

SyntaxResult<TypeBase>
Parser::ParseDeclResultType(ParsingDeclCollector &collector, Diag<> diagID) {
  return ParseType(collector, diagID);
}

SyntaxResult<TypeBase> Parser::ParseBasicType(ParsingDeclCollector &collector,
                                              Diag<> diagID) {

  assert(GetTok().IsBasicType());

  TypeQualifierList *qualifiers = nullptr;
  if (collector.GetTypeQualifierCollector().HasAny()) {
    qualifiers = TypeQualifierList::Create(
        collector.GetTypeQualifierCollector().GetTypeQualifiers(),
        GetSyntaxContext());
  }

  // Collect the type -- only basic types for now (TODO: user type  and function
  // types)

  auto status = CollectBasicTypeDecl(collector);
  if (status.HasCodeCompletion()) {
    // TODO: nothing to do
  }

  if (!collector.GetTypeSpecifierCollector().HasAny()) {
    // TODO: nothing to do
  }

  CollectTypeChunks(collector);
  TypeChunkList *chunks = nullptr;
  if (collector.GetTypeChunkCollector().HasAny()) {
    chunks = TypeChunkList::Create(
        collector.GetTypeChunkCollector().GetTypeChunks(), GetSyntaxContext());
  }

  TypeBase *ty = nullptr;
  if (collector.GetTypeSpecifierCollector().IsInt()) {
    ty = IntegerType::Create(NumberBitWidth::Platform, qualifiers, chunks,
                             GetSyntaxContext());
  }
  return syn::MakeSyntaxResult<TypeBase>(ty);
}

void Parser::ParseIdentifierType(TypeSpecifierCollector &collector,
                                 Diag<> diagID) {}
