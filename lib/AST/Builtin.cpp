#include "stone/AST/Builtin.h"
#include "stone/AST/ASTContext.h"
#include "stone/Basic/Memory.h"

using namespace stone;

// BuiltinTypeCache::BuiltinTypeCache(ASTContext &astContext)
//     : astContext(astContext),
//       BuiltinVoidType(new(astContext) VoidType(astContext)),
//       BuiltinNullType(new(astContext) NullType(astContext)),
//       BuiltinBoolType(new(astContext) BoolType(astContext)),
//       BuiltinFloat16Type(new(astContext) Float16Type(astContext)),
//       BuiltinFloat32Type(new(astContext) Float32Type(astContext)),
//       BuiltinFloat64Type(new(astContext) Float64Type(astContext)),
//       BuiltinFloat128Type(new(astContext) Float128Type(astContext)),
//       BuiltinFloatType(new(astContext) FloatType(astContext)),
//       BuiltinInt8Type(new(astContext) Int8Type(astContext)),
//       BuiltinInt16Type(new(astContext) Int16Type(astContext)),
//       BuiltinInt32Type(new(astContext) Int32Type(astContext)),
//       BuiltinInt64Type(new(astContext) Int64Type(astContext)),
//       BuiltinInt128Type(new(astContext) Int128Type(astContext)),
//       BuiltinIntType(new(astContext) IntType(astContext)),
//       BuiltinUInt8Type(new(astContext) UInt8Type(astContext)),
//       BuiltinUInt16Type(new(astContext) UInt16Type(astContext)),
//       BuiltinUInt32Type(new(astContext) UInt32Type(astContext)),
//       BuiltinUInt64Type(new(astContext) UInt64Type(astContext)),
//       BuiltinUInt128Type(new(astContext) UInt128Type(astContext)),
//       BuiltinUIntType(new(astContext) UIntType(astContext)) {}

Builtin::Builtin() {}