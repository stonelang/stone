#include "stone/Diag/SyntaxDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxNode.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

// fun Do() -> '() -> int'
Type Parser::ParseFunctionType(TypeCollector &collector, Diag<> diagID) {

  Type result;
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

  return result;
}

// Similar to ParseDeclSpecifiers
Type Parser::ParseType(TypeCollector &collector, Diag<> diagID) {
  Type result;
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

Type Parser::ParseDeclResultType(TypeCollector &collector, Diag<> diagID) {
  return ParseType(collector, diagID);
}

Type Parser::ParseBasicType(TypeCollector &collector, Diag<> diagID) {

  assert(GetTok().IsBasicType());
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

  Type ty;
  // TypeBase *ty = nullptr;
  switch (collector.GetTypeSpecifierCollector().GetKind()) {

  case TypeSpecifierKind::Void: {
    assert(collector.GetTypeSpecifierCollector().IsVoid());
    ty = GetSyntaxContext().GetBuiltinContext().BuiltinVoidType;
    break;
  }

  case TypeSpecifierKind::Int: {
    assert(collector.GetTypeSpecifierCollector().IsInt());
    ty = GetSyntaxContext().GetBuiltinContext().BuiltinIntType;
    break;
  }
  case TypeSpecifierKind::Int16: {
    assert(collector.GetTypeSpecifierCollector().IsInt16());
    ty = GetSyntaxContext().GetBuiltinContext().BuiltinInt16Type;
    break;
  }
  case TypeSpecifierKind::Int32: {
    assert(collector.GetTypeSpecifierCollector().IsInt32());
    ty = GetSyntaxContext().GetBuiltinContext().BuiltinInt32Type;
    break;
  }
  case TypeSpecifierKind::Int64: {
    assert(collector.GetTypeSpecifierCollector().IsInt64());
    ty = GetSyntaxContext().GetBuiltinContext().BuiltinInt64Type;
    break;
  }
  }
  // TODO: OK FOR NOW
  assert(!ty.IsNull());

  if (collector.GetTypeQualifierCollector().HasAny()) {
    ty.SetTypeQualifiers(collector.GetTypeQualifierCollector().GetTypeQualifiers());
  }
  if (chunks) {
    ty.SetTypeChunks(chunks);
  }
  return ty;
}

Type Parser::ParseIdentifierType(TypeCollector &collector, Diag<> diagID) {
  Type result;
  return result;
}
