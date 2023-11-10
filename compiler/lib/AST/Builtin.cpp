#include "stone/AST/Builtin.h"
#include "stone/AST/ASTAllocation.h"

using namespace stone;

Builtin::Builtin(ASTContext &astContext)
    : astContext(astContext),

      BuiltinVoidType(VoidType::Create(astContext, AllocationArena::Permanent,
                                       TypeQualifiers())),
      BuiltinNullType(NullType::Create(astContext, AllocationArena::Permanent)),
      BuiltinBoolType(BoolType::Create(astContext, AllocationArena::Permanent, TypeQualifiers())),

      BuiltinFloat16Type(FloatType::Create(NumberBitWidth::N16, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())),
      BuiltinFloat32Type(FloatType::Create(NumberBitWidth::N32, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())),
      BuiltinFloat64Type(FloatType::Create(NumberBitWidth::N64, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())),
      BuiltinFloat128Type(FloatType::Create(NumberBitWidth::N128, astContext,
                                            AllocationArena::Permanent,
                                            TypeQualifiers())),
      BuiltinFloatType(FloatType::Create(NumberBitWidth::Platform, astContext,
                                         AllocationArena::Permanent,
                                         TypeQualifiers())),

      BuiltinInt8Type(IntegerType::Create(NumberBitWidth::N16, astContext,
                                          AllocationArena::Permanent,
                                          TypeQualifiers())),
      BuiltinInt16Type(IntegerType::Create(NumberBitWidth::N16, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())),
      BuiltinInt32Type(IntegerType::Create(NumberBitWidth::N32, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())),
      BuiltinInt64Type(IntegerType::Create(NumberBitWidth::N64, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())),
      BuiltinInt128Type(IntegerType::Create(NumberBitWidth::N128, astContext,
                                            AllocationArena::Permanent,
                                            TypeQualifiers())),
      BuiltinIntType(IntegerType::Create(NumberBitWidth::Platform, astContext,
                                         AllocationArena::Permanent,
                                         TypeQualifiers())),

      BuiltinUInt8Type(UIntegerType::Create(NumberBitWidth::N8, astContext,
                                            AllocationArena::Permanent,
                                            TypeQualifiers())),
      BuiltinUInt16Type(UIntegerType::Create(NumberBitWidth::N16, astContext,
                                             AllocationArena::Permanent,
                                             TypeQualifiers())),
      BuiltinUInt32Type(UIntegerType::Create(NumberBitWidth::N32, astContext,
                                             AllocationArena::Permanent,
                                             TypeQualifiers())),
      BuiltinUInt64Type(UIntegerType::Create(NumberBitWidth::N64, astContext,
                                             AllocationArena::Permanent,
                                             TypeQualifiers())),
      BuiltinUInt128Type(UIntegerType::Create(NumberBitWidth::N128, astContext,
                                              AllocationArena::Permanent,
                                              TypeQualifiers())),
      BuiltinUIntType(UIntegerType::Create(NumberBitWidth::Platform, astContext,
                                           AllocationArena::Permanent,
                                           TypeQualifiers())) {}

void Builtin::Initialize() {}

Builtin::~Builtin() {}
