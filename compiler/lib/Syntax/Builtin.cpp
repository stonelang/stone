#include "stone/Syntax/Builtin.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

Builtin::Builtin(SyntaxContext &sc)
    : sc(sc),

      BuiltinFloat16Type(new (sc, AllocationArena::Permanent)
                             FloatType(BitWidth::BitWidth16, sc)),
      BuiltinFloat32Type(new (sc, AllocationArena::Permanent)
                             FloatType(BitWidth::BitWidth32, sc)),
      BuiltinFloat64Type(new (sc, AllocationArena::Permanent)
                             FloatType(BitWidth::BitWidth64, sc)),
      BuiltinFloat128Type(new (sc, AllocationArena::Permanent)
                              FloatType(BitWidth::BitWidth128, sc)),
      BuiltinFloatType(new (sc, AllocationArena::Permanent)
                           FloatType(BitWidth::Platform, sc)),

      BuiltinInt8Type(new (sc, AllocationArena::Permanent)
                          IntegerType(BitWidth::BitWidth16, sc)),
      BuiltinInt16Type(new (sc, AllocationArena::Permanent)
                           IntegerType(BitWidth::BitWidth16, sc)),
      BuiltinInt32Type(new (sc, AllocationArena::Permanent)
                           IntegerType(BitWidth::BitWidth32, sc)),
      BuiltinInt64Type(new (sc, AllocationArena::Permanent)
                           IntegerType(BitWidth::BitWidth64, sc)),
      BuiltinInt128Type(new (sc, AllocationArena::Permanent)
                            IntegerType(BitWidth::BitWidth128, sc)),
      BuiltinIntType(new (sc, AllocationArena::Permanent)
                         IntegerType(BitWidth::Platform, sc)) {}

Builtin::~Builtin() {}
