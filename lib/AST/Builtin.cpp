#include "stone/AST/Builtin.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Type.h"
#include "stone/AST/TypeState.h"
#include "stone/Basic/Memory.h"

using namespace stone;

Builtin::Builtin(const ASTContext &astContext) {

  // Create VoidType
  BuiltinVoidType =
      new (astContext) VoidType(new (astContext) BuiltinTypeState(astContext));

  // Create NullType
  BuiltinNullType =
      new (astContext) NullType(new (astContext) BuiltinTypeState(astContext));

  // Create BoolType
  BuiltinBoolType =
      new (astContext) BoolType(new (astContext) BuiltinTypeState(astContext));

  // Create Float16Type
  BuiltinFloat16Type = new (astContext)
      Float16Type(new (astContext) BuiltinTypeState(astContext));

  // Create Float32Type
  BuiltinFloat32Type = new (astContext)
      Float32Type(new (astContext) BuiltinTypeState(astContext));

  // Create Float64Type
  BuiltinFloat64Type = new (astContext)
      Float64Type(new (astContext) BuiltinTypeState(astContext));

  // Create Float128Type
  BuiltinFloat128Type = new (astContext)
      Float128Type(new (astContext) BuiltinTypeState(astContext));

  // Create FloatType
  BuiltinFloatType =
      new (astContext) FloatType(new (astContext) BuiltinTypeState(astContext));

  // Create Int8Type
  BuiltinInt8Type =
      new (astContext) Int8Type(new (astContext) BuiltinTypeState(astContext));

  // Create Int16Type
  BuiltinInt16Type =
      new (astContext) Int16Type(new (astContext) BuiltinTypeState(astContext));

  // Create Int32Type
  BuiltinInt32Type =
      new (astContext) Int32Type(new (astContext) BuiltinTypeState(astContext));

  // Create Int64Type
  BuiltinInt64Type =
      new (astContext) Int64Type(new (astContext) BuiltinTypeState(astContext));

  // Create Int128Type
  BuiltinInt128Type = new (astContext)
      Int128Type(new (astContext) BuiltinTypeState(astContext));

  // Create IntType
  BuiltinIntType =
      new (astContext) IntType(new (astContext) BuiltinTypeState(astContext));

  // Create UInt8Type
  BuiltinUInt8Type =
      new (astContext) UInt8Type(new (astContext) BuiltinTypeState(astContext));

  // Create UInt16Type
  BuiltinUInt16Type = new (astContext)
      UInt16Type(new (astContext) BuiltinTypeState(astContext));

  // Create UInt32Type
  BuiltinUInt32Type = new (astContext)
      UInt32Type(new (astContext) BuiltinTypeState(astContext));

  // Create UInt64Type
  BuiltinUInt64Type = new (astContext)
      UInt64Type(new (astContext) BuiltinTypeState(astContext));

  // Create UInt128Type
  BuiltinUInt128Type = new (astContext)
      UInt128Type(new (astContext) BuiltinTypeState(astContext));

  // Create UIntType
  BuiltinUIntType =
      new (astContext) UIntType(new (astContext) BuiltinTypeState(astContext));

  // Create UIntType
  BuiltinUIntType =
      new (astContext) UIntType(new (astContext) BuiltinTypeState(astContext));
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