#include "stone/Parse/Parser.h"
#include "stone/Parse/ParsingDeclSpec.h"
#include "stone/Parse/ParsingTypeSpec.h"

using namespace stone;

bool ParsingTypeSpec::IsTypeNull() {}

void ParsingTypeSpec::SetType(QualType ty) {}

bool ParsingBuiltinTypeSpec::IsInt8Type() const {
  return IsTypeKind(TypeKind::Int8);
}

bool ParsingBuiltinTypeSpec::IsInt16Type() const {
  return IsTypeKind(TypeKind::Int16);
}

bool ParsingBuiltinTypeSpec::IsInt32Type() const {
  return IsTypeKind(TypeKind::Int32);
}

bool ParsingBuiltinTypeSpec::IsInt64Type() const {
  return IsTypeKind(TypeKind::Int64);
}

bool ParsingBuiltinTypeSpec::IsInt128Type() const {
  return IsTypeKind(TypeKind::Int128);
}

bool ParsingBuiltinTypeSpec::IsUIntType() const {
  return IsTypeKind(TypeKind::UInt);
}

bool ParsingBuiltinTypeSpec::IsUInt8Type() const {
  return IsTypeKind(TypeKind::UInt8);
}

bool ParsingBuiltinTypeSpec::IsUInt16Type() const {
  return IsTypeKind(TypeKind::UInt16);
}

bool ParsingBuiltinTypeSpec::IsUInt32Type() const {
  return IsTypeKind(TypeKind::UInt32);
}

bool ParsingBuiltinTypeSpec::IsUInt64Type() const {
  return IsTypeKind(TypeKind::UInt64);
}

bool ParsingBuiltinTypeSpec::IsUInt128Type() const {
  return IsTypeKind(TypeKind::UInt128);
}

bool ParsingBuiltinTypeSpec::IsBoolType() const {
  return IsTypeKind(TypeKind::Bool);
}

bool ParsingBuiltinTypeSpec::IsCharType() const {
  return IsTypeKind(TypeKind::Char);
}

bool ParsingBuiltinTypeSpec::IsChar8Type() const {
  return IsTypeKind(TypeKind::Char8);
}

bool ParsingBuiltinTypeSpec::IsChar16Type() const {
  return IsTypeKind(TypeKind::Char16);
}

bool ParsingBuiltinTypeSpec::IsChar32Type() const {
  return IsTypeKind(TypeKind::Char32);
}

bool ParsingBuiltinTypeSpec::IsRealType() const {
  return IsTypeKind(TypeKind::Real);
}

bool ParsingBuiltinTypeSpec::IsFloatType() const {
  return IsTypeKind(TypeKind::Float);
}

bool ParsingBuiltinTypeSpec::IsFloat16Type() const {
  return IsTypeKind(TypeKind::Float16);
}

bool ParsingBuiltinTypeSpec::IsFloat32Type() const {
  return IsTypeKind(TypeKind::Float32);
}

bool ParsingBuiltinTypeSpec::IsFloat64Type() const {
  return IsTypeKind(TypeKind::Float64);
}

bool ParsingBuiltinTypeSpec::IsFloat128Type() const {
  return IsTypeKind(TypeKind::Float128);
}

bool ParsingBuiltinTypeSpec::IsComplex32Type() const {
  return IsTypeKind(TypeKind::Complex32);
}

bool ParsingBuiltinTypeSpec::IsComplex64Type() const {
  return IsTypeKind(TypeKind::Complex64);
}

bool ParsingBuiltinTypeSpec::IsImaginary32Type() const {
  return IsTypeKind(TypeKind::Imaginary32);
}

bool ParsingBuiltinTypeSpec::IsImaginary64Type() const {
  return IsTypeKind(TypeKind::Imaginary64);
}

bool ParsingBuiltinTypeSpec::IsNullType() const {
  return IsTypeKind(TypeKind::Null);
}
bool ParsingBuiltinTypeSpec::IsAutoType() const {
  return IsTypeKind(TypeKind::Auto);
}
bool ParsingBuiltinTypeSpec::IsVoidType() const {
  return IsTypeKind(TypeKind::Void);
}

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

ParserResult<ParsingTypeSpec> Parser::ParseType() {
  return ParseType(diag::expected_type);
}

ParserResult<ParsingTypeSpec> Parser::ParseType(Diag<> diagID) {

  if (GetCurTok().IsBuiltinType()) {
    return ParseBuiltinType(diagID);
  }

  if (GetCurTok().IsFun()) {
    return ParseFunctionType(diagID);
  }
}

ParserResult<ParsingTypeSpec> Parser::ParseBuiltinType(Diag<> diagID) {

  assert(GetCurTok().IsBuiltinType() &&
         "ParseBuiltinType requires a builtin-type token");

  auto builtinTypeKind = ResolveBuiltinTypeKind(GetCurTok().GetKind());
  auto builtinParsingTypeSpec =
      CreateParsingBuiltinTypeSpec(builtinTypeKind, ConsumeToken());
  builtinParsingTypeSpec->SetType(ResolveBuiltinType(builtinTypeKind));
  return stone::MakeParserResult<ParsingBuiltinTypeSpec>(
      builtinParsingTypeSpec);
}

ParserResult<ParsingTypeSpec> Parser::ParseFunctionType(Diag<> diagID) {

  assert(GetCurTok().IsFun() && "ParseFunctionType requires a fun-type token");
  auto parsingFunTypeSpec = CreateParsingFunTypeSpec(ConsumeToken());
  return stone::MakeParserResult<ParsingFunTypeSpec>(parsingFunTypeSpec);
}

QualType Parser::ResolveBuiltinType(TypeKind typeKind) {
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

ParsingBuiltinTypeSpec *Parser::CreateParsingBuiltinTypeSpec(TypeKind kind,
                                                             SrcLoc loc) {
  return new (*this) ParsingBuiltinTypeSpec(kind, loc);
}

ParsingFunTypeSpec *Parser::CreateParsingFunTypeSpec(SrcLoc loc) {

  return new (*this) ParsingFunTypeSpec(loc);
}
ParsingStructTypeSpec *Parser::CreateParsingStructTypeSpec(SrcLoc loc) {

  return new (*this) ParsingStructTypeSpec(loc);
}

ParsingIdentifierTypeSpec *Parser::CreateParsingIdentifierTypeSpec(SrcLoc loc) {

  return new (*this) ParsingIdentifierTypeSpec(loc);
}
