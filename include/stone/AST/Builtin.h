#ifndef STONE_AST_BUILTIN_H
#define STONE_AST_BUILTIN_H

#include "stone/AST/Identifier.h"
#include "stone/AST/Type.h"

namespace stone {

struct Builtin final {

  const Type *BuiltinFloat16Type;  /// 32-bit IEEE floating point
  const Type *BuiltinFloat32Type;  /// 32-bit IEEE floating point
  const Type *BuiltinFloat64Type;  /// 64-bit IEEE floating point
  const Type *BuiltinFloat128Type; /// 128-bit IEEE floating point
  const Type *BuiltinFloatType;    /// 128-bit IEEE floating point

  const Type *BuiltinInt8Type;
  const Type *BuiltinInt16Type;
  const Type *BuiltinInt32Type;
  const Type *BuiltinInt64Type;
  const Type *BuiltinInt128Type;
  const Type *BuiltinIntType;

  const Type *BuiltinUInt8Type;
  const Type *BuiltinUInt16Type;
  const Type *BuiltinUInt32Type;
  const Type *BuiltinUInt64Type;
  const Type *BuiltinUInt128Type;
  const Type *BuiltinUIntType;

  const Type *BuiltinAnyType;
  const Type *BuiltinVoidType;
  const Type *BuiltinNullType;
  const Type *BuiltinBoolType;

  // Type GetType(llvm::StringRef name);

  // Declare the set of builtin identifiers.
#define BUILTIN_IDENTIFIER_WITH_NAME(Name, IdStr)                              \
  Identifier Builtin##Name##Identifier;
#include "stone/AST/BuiltinIdentifiers.def"

  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;
  Builtin(const ASTContext &AC);
};

} // namespace stone
#endif
