#include "stone/Syntax/BuiltinContext.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

BuiltinContext::BuiltinContext(SyntaxContext &sc)
    : sc(sc),

      BuiltinFloat16Type(new(sc, AllocationArena::Permanent) FloatType(
          NumberBitWidth::N16, nullptr, nullptr, sc)),
      BuiltinFloat32Type(new(sc, AllocationArena::Permanent) FloatType(
          NumberBitWidth::N32, nullptr, nullptr, sc)),
      BuiltinFloat64Type(new(sc, AllocationArena::Permanent) FloatType(
          NumberBitWidth::N64, nullptr, nullptr, sc)),
      BuiltinFloat128Type(new(sc, AllocationArena::Permanent) FloatType(
          NumberBitWidth::N128, nullptr, nullptr, sc)),
      BuiltinFloatType(new(sc, AllocationArena::Permanent) FloatType(
          NumberBitWidth::Platform, nullptr, nullptr, sc)),

      BuiltinInt8Type(new(sc, AllocationArena::Permanent) IntegerType(
          NumberBitWidth::N16, nullptr, nullptr, sc)),
      BuiltinInt16Type(new(sc, AllocationArena::Permanent) IntegerType(
          NumberBitWidth::N16, nullptr, nullptr, sc)),
      BuiltinInt32Type(new(sc, AllocationArena::Permanent) IntegerType(
          NumberBitWidth::N32, nullptr, nullptr, sc)),
      BuiltinInt64Type(new(sc, AllocationArena::Permanent) IntegerType(
          NumberBitWidth::N64, nullptr, nullptr, sc)),
      BuiltinInt128Type(new(sc, AllocationArena::Permanent) IntegerType(
          NumberBitWidth::N128, nullptr, nullptr, sc)),
      BuiltinIntType(new(sc, AllocationArena::Permanent) IntegerType(
          NumberBitWidth::Platform, nullptr, nullptr, sc)),

      BuiltinUInt8Type(new(sc, AllocationArena::Permanent) UIntegerType(
          NumberBitWidth::N8, nullptr, nullptr, sc)),
      BuiltinUInt16Type(new(sc, AllocationArena::Permanent) UIntegerType(
          NumberBitWidth::N16, nullptr, nullptr, sc)),
      BuiltinUInt32Type(new(sc, AllocationArena::Permanent) UIntegerType(
          NumberBitWidth::N32, nullptr, nullptr, sc)),
      BuiltinUInt64Type(new(sc, AllocationArena::Permanent) UIntegerType(
          NumberBitWidth::N64, nullptr, nullptr, sc)),
      BuiltinUInt128Type(new(sc, AllocationArena::Permanent) UIntegerType(
          NumberBitWidth::N128, nullptr, nullptr, sc)),
      BuiltinUIntType(new(sc, AllocationArena::Permanent) UIntegerType(
          NumberBitWidth::Platform, nullptr, nullptr, sc)) {}

BuiltinContext::~BuiltinContext() {}
