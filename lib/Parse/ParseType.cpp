#include "stone/AST/TypeState.h"
#include "stone/Parse/Parser.h"

using namespace stone;

TypeKind Parser::ResolveBuiltinTypeKind(tok kind) {
  switch (kind) {
  case tok::kw_void:
    return TypeKind::Void;
  case tok::kw_null:
    return TypeKind::Null;
  case tok::kw_auto:
    return TypeKind::Auto;
  case tok::kw_char:
    return TypeKind::Char;
  case tok::kw_char8:
    return TypeKind::Char8;
  case tok::kw_char16:
    return TypeKind::Char16;
  case tok::kw_char32:
    return TypeKind::Char32;
  case tok::kw_int:
    return TypeKind::Int;
  case tok::kw_int8:
    return TypeKind::Int8;
  case tok::kw_int16:
    return TypeKind::Int16;
  case tok::kw_int32:
    return TypeKind::Int32;
  case tok::kw_int64:
    return TypeKind::Int64;
  case tok::kw_uint:
    return TypeKind::UInt;
  case tok::kw_uint8:
    return TypeKind::UInt8;
  case tok::kw_uint16:
    return TypeKind::UInt16;
  case tok::kw_uint32:
    return TypeKind::UInt32;
  case tok::kw_uint64:
    return TypeKind::UInt64;
  case tok::kw_float:
    return TypeKind::Float;
  case tok::kw_float32:
    return TypeKind::Float32;
  case tok::kw_float64:
    return TypeKind::Float64;
  case tok::kw_complex32:
    return TypeKind::Complex32;
  case tok::kw_complex64:
    return TypeKind::Complex64;
  case tok::kw_imaginary32:
    return TypeKind::Imaginary32;
  case tok::kw_imaginary64:
    return TypeKind::Imaginary64;
  }
  llvm_unreachable("Unable to resolve token-kind to a type-kind");
}

ParserResult<TypeState> Parser::ParseType() {
  return ParseType(diag::expected_type);
}

ParserResult<TypeState> Parser::ParseType(Diag<> message) {

  if (GetCurTok().IsBuiltin()) {
    return ParseBuiltinType(message);
  }

  if (GetCurTok().IsFun()) {
    return ParseFunctionType(message);
  }
}

ParserStatus Parser::ParseTypeModifiers(TypeInfluencerList &modifiers) {
  ParserStatus status;
  while (IsParsing()) {
    switch (GetCurTok().GetKind()) {
    case tok::kw_const: {
      modifiers.AddConst(ConsumeToken());
      continue;
    }
    case tok::kw_pure: {
      modifiers.AddPure(ConsumeToken());
      continue;
    }
    case tok::kw_stone: {
      modifiers.AddStone(ConsumeToken());
      continue;
    }
    case tok::kw_volatile: {
      modifiers.AddVolatile(ConsumeToken());
      continue;
    }
    default:
      break;
    }
    return status;
  }
  return status;
}

ParserStatus Parser::ParseTypeAttributes(TypeInfluencerList &attributes) {
  ParserStatus status;
  return status;
}

ParserResult<TypeState> Parser::ParseDeclResultType(Diag<> message) {
  /// TODO: There is more -- return ParseType for now
  return ParseType(message);
}

ParserResult<TypeState> Parser::ParseBuiltinType(Diag<> message) {

  assert(GetCurTok().IsBuiltin() &&
         "ParseBuiltinType requires a builtin-type token");

  // Check for modifiers
  TypeInfluencerList modifiers(GetASTContext());
  ParseTypeModifiers(modifiers);

  // auto builtinTypeKind =
  // Parser::ResolveBuiltinTypeKind(GetCurTok().GetKind()); auto builtinType =
  // ResolveBuiltinType(builtinTypeKind); return
  // stone::MakeParserResult<BuiltinTypeState>(builtinType->GetState());
}

ParserResult<TypeState> Parser::ParseFunctionType(Diag<> diagID) {

  assert(GetCurTok().IsFun() && "ParseFunctionType requires a fun-type token");
  // auto functionTypeState =
  //     new (GetASTContext()) FunctionTypeState(ConsumeToken());
  // return stone::MakeParserResult<FunctionTypeState>(funTypeState);
}

const Type *Parser::ResolveBuiltinType(TypeKind typeKind) {
  switch (typeKind) {
  case TypeKind::Int:
    return GetASTContext().GetBuiltin().BuiltinIntType;
  case TypeKind::Int8:
    return GetASTContext().GetBuiltin().BuiltinInt8Type;
  case TypeKind::Int16:
    return GetASTContext().GetBuiltin().BuiltinInt16Type;
  case TypeKind::Int32:
    return GetASTContext().GetBuiltin().BuiltinInt32Type;
  case TypeKind::Int64:
    return GetASTContext().GetBuiltin().BuiltinInt32Type;
  }
  llvm_unreachable("Unable to resolve builtin-type");
}
