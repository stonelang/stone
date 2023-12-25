#include "stone/Diag/ASTDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTNode.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/Types.h"

using namespace stone;

// fun Do() -> '() -> int'
Type Parser::ParseFunctionType(ParsingDeclSpecifierCollector &collector,
                               Diag<> diagID) {

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
  // CollectTypeThunks(collector);

  // // Requires at least a direct type pattern which is just the type by
  // itself. assert(collector.GetTypeThunkCollector().HasAny());

  // TODO: Call parseType to get the actual type

  // auto qualType = new (GetASTContext()) QualifierType();
  // auto functionType = new (GetASTContext()) FunctionType(qualType);

  return result;
}

// Similar to ParseDeclSpecifiers
Type Parser::ParseType(ParsingDeclSpecifierCollector &collector,
                       Diag<> diagID) {
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
  //   Type qualType(type);
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

Type Parser::ParseDeclResultType(ParsingDeclSpecifierCollector &collector,
                                 Diag<> diagID) {
  return ParseType(collector, diagID);
}

Type Parser::ParseBasicType(ParsingDeclSpecifierCollector &collector,
                            Diag<> diagID) {

  assert(GetTok().IsBasicType());
  // Collect the type -- only basic types for now (TODO: user type  and function
  // types)

  auto status = CollectBasicTypeSpecifier(collector);
  if (status.HasCodeCompletion()) {
    // TODO: nothing to do
  }

  if (!collector.GetTypeSpecifierCollector().HasAny()) {
    // TODO: nothing to do
  }

  CollectTypeThunks(collector);

  Type result;
  switch (collector.GetTypeSpecifierCollector().GetSpecifierKind()) {

  case TypeSpecifierKind::Void: {
    assert(collector.GetTypeSpecifierCollector().IsVoid());
    result = GetASTContext().GetBuiltin().BuiltinVoidType;
    break;
  }

  case TypeSpecifierKind::Int: {
    assert(collector.GetTypeSpecifierCollector().IsInt());
    result = GetASTContext().GetBuiltin().BuiltinIntType;
    break;
  }
  case TypeSpecifierKind::Int16: {
    assert(collector.GetTypeSpecifierCollector().IsInt16());
    result = GetASTContext().GetBuiltin().BuiltinInt16Type;
    break;
  }
  case TypeSpecifierKind::Int32: {
    assert(collector.GetTypeSpecifierCollector().IsInt32());
    result = GetASTContext().GetBuiltin().BuiltinInt32Type;
    break;
  }
  case TypeSpecifierKind::Int64: {
    assert(collector.GetTypeSpecifierCollector().IsInt64());
    result = GetASTContext().GetBuiltin().BuiltinInt64Type;
    break;
  }
  }
  // TODO: OK FOR NOW
  assert(!result.IsNull());

  // TODO: Apply qualifiers

  if (collector.GetTypeQualifierCollector().HasAny()) {
    collector.GetTypeQualifierCollector().Apply(result);
  }
  // if (collector.GetTypeThunkCollector().HasAny()) {
  //   ty.SetTypeThunkCollector(collector.GetTypeThunkCollector());
  // }
  return result;
}

Type Parser::ParseIdentifierType(ParsingDeclSpecifierCollector &collector,
                                 Diag<> diagID) {
  Type result;
  return result;
}
