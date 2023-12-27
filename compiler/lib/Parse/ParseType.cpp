#include "stone/Diag/ASTDiagnostic.h"
#include "stone/Parse/Parser.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ASTNode.h"
#include "stone/Syntax/Type.h"
#include "stone/Syntax/Types.h"

using namespace stone;

// fun Do() -> '() -> int'
Type Parser::ParseFunctionType(ParsingDecl &collector, Diag<> diagID) {

  Type result;
  ParsingScope functionTypeScope(*this, ScopeKind::FunctionType,
                                 "parsing function-type");

  // assert(collector.GetFunctionSpecifierCollector().HasFun() && "Function is
  // missing 'fun' ");
  // assert(collector.GetFunctionSpecifierCollector().HasArrow());

  // assert(collector.GetTypeSpecifierCollector().HasNone() && "Function
  // type-specifier comes after ->");

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

  // auto qualType = new (GetASTContext()) QualifierType();
  // auto functionType = new (GetASTContext()) FunctionType(qualType);

  return result;
}

// Similar to ParseDeclSpecifiers
Type Parser::ParseType(ParsingDecl &collector, Diag<> diagID) {
  Type result;
  ParsingScope typeScope(*this, ScopeKind::Type, "parsing type");

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

Type Parser::ParseDeclResultType(ParsingDecl &collector, Diag<> diagID) {
  return ParseType(collector, diagID);
}

Type Parser::ParseBasicType(ParsingDecl &collector, Diag<> diagID) {

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

  CollectTypeChunks(collector);

  Type result =
      GetBasicType(collector.GetTypeSpecifierCollector().GetSpecifierKind());

  // TODO: Applying now -- may want to apply later
  if (collector.GetTypeQualifierCollector().HasAny()) {
    collector.GetTypeQualifierCollector().Apply(result);
  }
  if (collector.GetTypeChunkCollector().HasAny()) {
    result.SetTypeChunkList(
        collector.GetTypeChunkCollector().CreateTypeChunkList(GetASTContext()));
  }
  return result;
}

Type Parser::GetBasicType(TypeSpecifierKind kind) {
  switch (kind) {
  case TypeSpecifierKind::Void:
    return GetASTContext().GetBuiltin().GetTypeCache().BuiltinVoidType;
  case TypeSpecifierKind::Int:
    return GetASTContext().GetBuiltin().GetTypeCache().BuiltinIntType;
  case TypeSpecifierKind::Int16:
    return GetASTContext().GetBuiltin().GetTypeCache().BuiltinInt16Type;
  case TypeSpecifierKind::Int32:
    return GetASTContext().GetBuiltin().GetTypeCache().BuiltinInt32Type;
  case TypeSpecifierKind::Int64:
    return GetASTContext().GetBuiltin().GetTypeCache().BuiltinInt64Type;
  default:
    llvm_unreachable("Unknown specifier type");
  }
}

Type Parser::ParseIdentifierType(ParsingDecl &collector, Diag<> diagID) {
  Type result;
  return result;
}
