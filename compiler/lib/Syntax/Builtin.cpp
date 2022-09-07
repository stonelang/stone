#include "stone/Syntax/Builtin.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

Builtin::Builtin(SyntaxContext &sc)
    : sc(sc), 

    Float16Type(new (sc, AllocationArena::Permanent) FloatType(BitWidthKind::BitWidth16, sc)),
    Float32Type(new (sc, AllocationArena::Permanent) FloatType(BitWidthKind::BitWidth32, sc)),
    Float64Type(new (sc, AllocationArena::Permanent) FloatType(BitWidthKind::BitWidth64, sc)),

    Int16Type(new (sc, AllocationArena::Permanent) IntegerType(BitWidthKind::BitWidth16, sc)),
    Int32Type(new (sc, AllocationArena::Permanent) IntegerType(BitWidthKind::BitWidth32, sc)),
   	Int64Type(new (sc, AllocationArena::Permanent) IntegerType(BitWidthKind::BitWidth64, sc)) {

    }

Builtin::~Builtin() {}
