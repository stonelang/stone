#include "stone/AST/Builtin.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Type.h"
#include "stone/Basic/Memory.h"

using namespace stone;
// clang-off
Builtin::Builtin(const ASTContext &AC)
    : BuiltinVoidType(new(AC) VoidType()), BuiltinNullType(new(AC) NullType()),
      BuiltinBoolType(new(AC) BoolType()),
      BuiltinFloat16Type(new(AC) Float16Type()),
      BuiltinFloat32Type(new(AC) Float32Type()),
      BuiltinFloat64Type(new(AC) Float64Type()),
      BuiltinFloat128Type(new(AC) Float128Type()),
      BuiltinFloatType(new(AC) FloatType()),
      BuiltinInt8Type(new(AC) Int8Type()),
      BuiltinInt16Type(new(AC) Int16Type()),
      BuiltinInt32Type(new(AC) Int32Type()),
      BuiltinInt64Type(new(AC) Int64Type()),
      BuiltinInt128Type(new(AC) Int128Type()),
      BuiltinIntType(new(AC) IntType()), BuiltinUInt8Type(new(AC) UInt8Type()),
      BuiltinUInt16Type(new(AC) UInt16Type()),
      BuiltinUInt32Type(new(AC) UInt32Type()),
      BuiltinUInt64Type(new(AC) UInt64Type()),
      BuiltinUInt128Type(new(AC) UInt128Type()),
      BuiltinUIntType(new(AC) UIntType()) {}
