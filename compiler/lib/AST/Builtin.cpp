#include "stone/AST/Builtin.h"
#include "stone/Basic/Memory.h"
#include "stone/AST/ASTContext.h"

using namespace stone;

BuiltinTypeCache::BuiltinTypeCache(ASTContext &astContext)
    : astContext(astContext),

      BuiltinAnyType(new(astContext, MemoryAllocationArena::Permanent)
                         AnyType(astContext)),
      BuiltinVoidType(new(astContext, MemoryAllocationArena::Permanent)
                          VoidType(astContext)),
      BuiltinNullType(new(astContext, MemoryAllocationArena::Permanent)
                          NullType(astContext)),
      BuiltinBoolType(new(astContext, MemoryAllocationArena::Permanent)
                          BoolType(astContext)),
      BuiltinFloat16Type(new(astContext, MemoryAllocationArena::Permanent)
                             FloatType(NumberBitWidth::N16, astContext)),
      BuiltinFloat32Type(new(astContext, MemoryAllocationArena::Permanent)
                             FloatType(NumberBitWidth::N32, astContext)),
      BuiltinFloat64Type(new(astContext, MemoryAllocationArena::Permanent)
                             FloatType(NumberBitWidth::N64, astContext)),
      BuiltinFloat128Type(new(astContext, MemoryAllocationArena::Permanent)
                              FloatType(NumberBitWidth::N128, astContext)),
      BuiltinFloatType(new(astContext, MemoryAllocationArena::Permanent)
                           FloatType(NumberBitWidth::Platform, astContext)),

      BuiltinInt8Type(new(astContext, MemoryAllocationArena::Permanent)
                          IntegerType(NumberBitWidth::N16, astContext)),
      BuiltinInt16Type(new(astContext, MemoryAllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N16, astContext)),
      BuiltinInt32Type(new(astContext, MemoryAllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N32, astContext)),
      BuiltinInt64Type(new(astContext, MemoryAllocationArena::Permanent)
                           IntegerType(NumberBitWidth::N64, astContext)),
      BuiltinInt128Type(new(astContext, MemoryAllocationArena::Permanent)
                            IntegerType(NumberBitWidth::N128, astContext)),
      BuiltinIntType(new(astContext, MemoryAllocationArena::Permanent)
                         IntegerType(NumberBitWidth::Platform, astContext)),

      BuiltinUInt8Type(new(astContext, MemoryAllocationArena::Permanent)
                           UIntegerType(NumberBitWidth::N8, astContext)),

      BuiltinUInt16Type(new(astContext, MemoryAllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N16, astContext)),

      BuiltinUInt32Type(new(astContext, MemoryAllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N32, astContext)),
      BuiltinUInt64Type(new(astContext, MemoryAllocationArena::Permanent)
                            UIntegerType(NumberBitWidth::N64, astContext)),
      BuiltinUInt128Type(new(astContext, MemoryAllocationArena::Permanent)
                             UIntegerType(NumberBitWidth::N128, astContext)),
      BuiltinUIntType(new(astContext, MemoryAllocationArena::Permanent)
                          UIntegerType(NumberBitWidth::Platform, astContext)) {}

BuiltinIdentifierCache::BuiltinIdentifierCache(ASTContext &astContext)
    : astContext(astContext) {

  // Initialize all of the known identifiers.
#define BUILTIN_IDENTIFIER_WITH_NAME(Name, IdStr)                              \
  Builtin##Name##Identifier = astContext.GetIdentifier(IdStr);
#include "stone/AST/BuiltinIdentifiers.def"
}

BuiltinFunctionCache::BuiltinFunctionCache(ASTContext &astContext)
    : astContext(astContext) {}

BuiltinDeclCache::BuiltinDeclCache(ASTContext &astContext)
    : astContext(astContext) {}

Builtin::Builtin(ASTContext &astContext)
    : astContext(astContext), typeCache(astContext),
      identifierCache(astContext), functionCache(astContext),
      declCache(astContext) {}

Builtin::~Builtin() {}
