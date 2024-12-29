#include "stone/AST/Builtin.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/Type.h"
#include "stone/Basic/Memory.h"

using namespace stone;

Builtin::Builtin(const ASTContext &AC)
    : BuiltinVoidType(new(AC) VoidType(AC)),
      BuiltinNullType(new(AC) NullType(AC)),
      BuiltinBoolType(new(AC) BoolType(AC)),
      BuiltinFloat16Type(new(AC) Float16Type(AC)),
      BuiltinFloat32Type(new(AC) Float32Type(AC)),
      BuiltinFloat64Type(new(AC) Float64Type(AC)),
      BuiltinFloat128Type(new(AC) Float128Type(AC)),
      BuiltinFloatType(new(AC) FloatType(AC)),
      BuiltinInt8Type(new(AC) Int8Type(AC)),
      BuiltinInt16Type(new(AC) Int16Type(AC)),
      BuiltinInt32Type(new(AC) Int32Type(AC)),
      BuiltinInt64Type(new(AC) Int64Type(AC)),
      BuiltinInt128Type(new(AC) Int128Type(AC)),
      BuiltinIntType(new(AC) IntType(AC)),
      Builtinuint8Type(new(AC) uint8Type(AC)),
      BuiltinUInt16Type(new(AC) UInt16Type(AC)),
      BuiltinUInt32Type(new(AC) UInt32Type(AC)),
      BuiltinUInt64Type(new(AC) UInt64Type(AC)),
      BuiltinUInt128Type(new(AC) UInt128Type(AC)),
      BuiltinUIntType(new(AC) UIntType(AC)) {}
