#include "stone/Syntax/BuiltinContext.h"
#include "stone/Basic/Mem.h"
#include "stone/Syntax/ASTAllocation.h"

using namespace stone;

BuiltinContext::BuiltinContext(ASTContext &sc)
    : sc(sc),

      BuiltinVoidType(VoidType::Create(sc)),
      BuiltinNullType(new(sc, AllocationArena::Permanent) NullType(sc)),
      BuiltinBoolType(new(sc, AllocationArena::Permanent) BoolType(sc)),
      BuiltinFloat16Type(new(sc, AllocationArena::Permanent)
                             FloatType(NumberBitWidth::N16, sc)),
      BuiltinFloat32Type(new(sc, AllocationArena::Permanent)
                             FloatType(NumberBitWidth::N32, sc)),
      BuiltinFloat64Type(new(sc, AllocationArena::Permanent)
                             FloatType(NumberBitWidth::N64, sc)),
      BuiltinFloat128Type(new(sc, AllocationArena::Permanent)
                              FloatType(NumberBitWidth::N128, sc)),
      BuiltinFloatType(new(sc, AllocationArena::Permanent)
                           FloatType(NumberBitWidth::Platform, sc)),

      BuiltinInt8Type(new(sc, AllocationArena::Permanent)
                          IntegerType(NumberBitWidth::N16, sc)),
      BuiltinInt16Type(new(sc, AllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N16, sc)),
      BuiltinInt32Type(new(sc, AllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N32, sc)),
      BuiltinInt64Type(new(sc, AllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N64, sc)),
      BuiltinInt128Type(new(sc, AllocationArena::Permanent)
                            IntegerType(NumberBitWidth::N128, sc)),
      BuiltinIntType(new(sc, AllocationArena::Permanent)
                         IntegerType(NumberBitWidth::Platform, sc)),

      BuiltinUInt8Type(new(sc, AllocationArena::Permanent)
                           UIntegerType(NumberBitWidth::N8, sc)),
      BuiltinUInt16Type(new(sc, AllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N16, sc)),
      BuiltinUInt32Type(new(sc, AllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N32, sc)),
      BuiltinUInt64Type(new(sc, AllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N64, sc)),
      BuiltinUInt128Type(new(sc, AllocationArena::Permanent)
                             UIntegerType(NumberBitWidth::N128, sc)),
      BuiltinUIntType(new(sc, AllocationArena::Permanent)
                          UIntegerType(NumberBitWidth::Platform, sc)) {}

void BuiltinContext::Initialize() {}

BuiltinContext::~BuiltinContext() {}
