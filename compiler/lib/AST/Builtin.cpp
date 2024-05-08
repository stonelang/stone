#include "stone/AST/Builtin.h"
#include "stone/AST/ASTContext.h"
#include "stone/Basic/Memory.h"

using namespace stone;

BuiltinTypeCache::BuiltinTypeCache(ASTContext &astContext)
    : astContext(astContext),

      BuiltinVoidType(new(astContext, MemoryAllocationArena::Permanent)
                          VoidType(astContext)),
      BuiltinNullType(new(astContext, MemoryAllocationArena::Permanent)
                          NullType(astContext)),
      BuiltinBoolType(new(astContext, MemoryAllocationArena::Permanent)
                          BoolType(astContext)),
      BuiltinFloat16Type(new(astContext, MemoryAllocationArena::Permanent)
                             Float16Type(astContext)),
      BuiltinFloat32Type(new(astContext, MemoryAllocationArena::Permanent)
                             Float32Type(astContext)),
      BuiltinFloat64Type(new(astContext, MemoryAllocationArena::Permanent)
                             Float64Type(astContext)),
      BuiltinFloat128Type(new(astContext, MemoryAllocationArena::Permanent)
                              Float128Type(astContext)),
      BuiltinFloatType(new(astContext, MemoryAllocationArena::Permanent)
                           FloatType(astContext)),

      BuiltinInt8Type(new(astContext, MemoryAllocationArena::Permanent)
                          Int8Type(astContext)),
      BuiltinInt16Type(new(astContext, MemoryAllocationArena::Permanent)
                           Int16Type(astContext)),
      BuiltinInt32Type(new(astContext, MemoryAllocationArena::Permanent)
                           Int32Type(astContext)),
      BuiltinInt64Type(new(astContext, MemoryAllocationArena::Permanent)
                           Int64Type(astContext)),
      BuiltinInt128Type(new(astContext, MemoryAllocationArena::Permanent)
                            Int128Type(astContext)),
      BuiltinIntType(new(astContext, MemoryAllocationArena::Permanent)
                         IntType(astContext)),

      BuiltinUInt8Type(new(astContext, MemoryAllocationArena::Permanent)
                           UInt8Type(astContext)),

      BuiltinUInt16Type(new(astContext, MemoryAllocationArena::Permanent)
                            UInt16Type(astContext)),

      BuiltinUInt32Type(new(astContext, MemoryAllocationArena::Permanent)
                            UInt32Type(astContext)),
      BuiltinUInt64Type(new(astContext, MemoryAllocationArena::Permanent)
                            UInt64Type(astContext)),
      BuiltinUInt128Type(new(astContext, MemoryAllocationArena::Permanent)
                             UInt128Type(astContext)),
      BuiltinUIntType(new(astContext, MemoryAllocationArena::Permanent)
                          UIntType(astContext)) {}

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
