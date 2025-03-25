#include "stone/AST/Builtin.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeState.h"
#include "stone/Basic/Memory.h"

using namespace stone;

Builtin::Builtin(const ASTContext &AC) {

  // Create VoidType
  BuiltinVoidType = new (AC) VoidType(new (AC) BuiltinTypeState());

  // Create NullType
  BuiltinNullType = new (AC) NullType(new (AC) BuiltinTypeState());

  // Create BoolType
  BuiltinBoolType = new (AC) BoolType(new (AC) BuiltinTypeState());

  // Create Float16Type
  BuiltinFloat16Type = new (AC) Float16Type(new (AC) BuiltinTypeState());

  // Create Float32Type
  BuiltinFloat32Type = new (AC) Float32Type(new (AC) BuiltinTypeState());

  // Create Float64Type
  BuiltinFloat64Type = new (AC) Float64Type(new (AC) BuiltinTypeState());

  // Create Float128Type
  BuiltinFloat128Type = new (AC) Float128Type(new (AC) BuiltinTypeState());

  // Create FloatType
  BuiltinFloatType = new (AC) FloatType(new (AC) BuiltinTypeState());

  // Create Int8Type
  BuiltinInt8Type = new (AC) Int8Type(new (AC) BuiltinTypeState());

  // Create Int16Type
  BuiltinInt16Type = new (AC) Int16Type(new (AC) BuiltinTypeState());

  // Create Int32Type
  BuiltinInt32Type = new (AC) Int32Type(new (AC) BuiltinTypeState());

  // Create Int64Type
  BuiltinInt64Type = new (AC) Int64Type(new (AC) BuiltinTypeState());

  // Create Int128Type
  BuiltinInt128Type = new (AC) Int128Type(new (AC) BuiltinTypeState());

  // Create IntType
  BuiltinIntType = new (AC) IntType(new (AC) BuiltinTypeState());

  // Create UInt8Type
  BuiltinUInt8Type = new (AC) UInt8Type(new (AC) BuiltinTypeState());

  // Create UInt16Type
  BuiltinUInt16Type = new (AC) UInt16Type(new (AC) BuiltinTypeState());

  // Create UInt32Type
  BuiltinUInt32Type = new (AC) UInt32Type(new (AC) BuiltinTypeState());

  // Create UInt64Type
  BuiltinUInt64Type = new (AC) UInt64Type(new (AC) BuiltinTypeState());

  // Create UInt128Type
  BuiltinUInt128Type = new (AC) UInt128Type(new (AC) BuiltinTypeState());

  // Create UIntType
  BuiltinUIntType = new (AC) UIntType(new (AC) BuiltinTypeState());

  // Create UIntType
  BuiltinUIntType = new (AC) UIntType(new (AC) BuiltinTypeState());
}

bool Builtin::IsBuiltinTypeKind(TypeKind kind) {
  switch (kind) {
  case TypeKind::Void:
  case TypeKind::Null:
  case TypeKind::Auto:
  case TypeKind::Char:
  case TypeKind::Char8:
  case TypeKind::Char16:
  case TypeKind::Char32:
  case TypeKind::Int:
  case TypeKind::Int8:
  case TypeKind::Int16:
  case TypeKind::Int32:
  case TypeKind::Int64:
  case TypeKind::UInt:
  case TypeKind::UInt8:
  case TypeKind::UInt16:
  case TypeKind::UInt32:
  case TypeKind::UInt64:
  case TypeKind::Float:
  case TypeKind::Float32:
  case TypeKind::Float64:
  case TypeKind::Complex32:
  case TypeKind::Complex64:
  case TypeKind::Imaginary32:
  case TypeKind::Imaginary64:
    return true;
  }
  return false;
}