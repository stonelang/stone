#include "stone/Syntax/Builtin.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/Types.h"

using namespace stone;
using namespace stone::syn;

Builtin::Builtin(SyntaxContext &sc)
    : sc(sc), Float16Type(new (sc, AllocationArena::Permanent) FloatType(FloatType::FloatPointBitWidth::BitWidth16, sc)) {


    }

Builtin::~Builtin() {}
