#include "stone/AST/Builtin.h"
#include "stone/AST/ASTAllocation.h"

using namespace stone;
using namespace stone::ast;

Builtin::Builtin(ASTContext &astContext)
    : astContext(astContext),

      BuiltinVoidType(VoidType::Create(astContext)),
      BuiltinNullType(new(astContext, AllocationArena::Permanent) NullType(astContext)),
      BuiltinBoolType(new(astContext, AllocationArena::Permanent) BoolType(astContext)),
      BuiltinFloat16Type(new(astContext, AllocationArena::Permanent)
                             FloatType(NumberBitWidth::N16, astContext)),
      BuiltinFloat32Type(new(astContext, AllocationArena::Permanent)
                             FloatType(NumberBitWidth::N32, astContext)),
      BuiltinFloat64Type(new(astContext, AllocationArena::Permanent)
                             FloatType(NumberBitWidth::N64, astContext)),
      BuiltinFloat128Type(new(astContext, AllocationArena::Permanent)
                              FloatType(NumberBitWidth::N128, astContext)),
      BuiltinFloatType(new(astContext, AllocationArena::Permanent)
                           FloatType(NumberBitWidth::Platform, astContext)),

      BuiltinInt8Type(new(astContext, AllocationArena::Permanent)
                          IntegerType(NumberBitWidth::N16, astContext)),
      BuiltinInt16Type(new(astContext, AllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N16, astContext)),
      BuiltinInt32Type(new(astContext, AllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N32, astContext)),
      BuiltinInt64Type(new(astContext, AllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N64, astContext)),
      BuiltinInt128Type(new(astContext, AllocationArena::Permanent)
                            IntegerType(NumberBitWidth::N128, astContext)),
      BuiltinIntType(new(astContext, AllocationArena::Permanent)
                         IntegerType(NumberBitWidth::Platform, astContext)),

      BuiltinUInt8Type(new(astContext, AllocationArena::Permanent)
                           UIntegerType(NumberBitWidth::N8, astContext)),
      BuiltinUInt16Type(new(astContext, AllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N16, astContext)),
      BuiltinUInt32Type(new(astContext, AllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N32, astContext)),
      BuiltinUInt64Type(new(astContext, AllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N64, astContext)),
      BuiltinUInt128Type(new(astContext, AllocationArena::Permanent)
                             UIntegerType(NumberBitWidth::N128, astContext)),
      BuiltinUIntType(new(astContext, AllocationArena::Permanent)
                          UIntegerType(NumberBitWidth::Platform, astContext)) {}

void Builtin::Initialize() {}

Builtin::~Builtin() {}
