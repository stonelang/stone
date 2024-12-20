#ifndef STONE_AST_BUILTIN_H
#define STONE_AST_BUILTIN_H

#include "stone/AST/Identifier.h"
#include "stone/AST/Type.h"

namespace stone {

class ASTContext;

class Builtin final {
  ASTContext &astContext;

public:
  const CanType BuiltinFloat16Type;  /// 32-bit IEEE floating point
  const CanType BuiltinFloat32Type;  /// 32-bit IEEE floating point
  const CanType BuiltinFloat64Type;  /// 64-bit IEEE floating point
  const CanType BuiltinFloat128Type; /// 128-bit IEEE floating point
  const CanType BuiltinFloatType;    /// 128-bit IEEE floating point

  const CanType BuiltinInt8Type;
  const CanType BuiltinInt16Type;
  const CanType BuiltinInt32Type;
  const CanType BuiltinInt64Type;
  const CanType BuiltinInt128Type;
  const CanType BuiltinIntType;

  const CanType BuiltinUInt8Type;
  const CanType BuiltinUInt16Type;
  const CanType BuiltinUInt32Type;
  const CanType BuiltinUInt64Type;
  const CanType BuiltinUInt128Type;
  const CanType BuiltinUIntType;

  const CanType BuiltinAnyType;
  const CanType BuiltinVoidType;
  const CanType BuiltinNullType;
  const CanType BuiltinBoolType;

  QualType GetType(llvm::StringRef name);

public:
  // Declare the set of builtin identifiers.
#define BUILTIN_IDENTIFIER_WITH_NAME(Name, IdStr)                              \
  Identifier Builtin##Name##Identifier;
#include "stone/AST/BuiltinIdentifiers.def"

public:
  // Decls

public:
  // Functions
public:
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

public:
  Builtin(ASTContext &astContext);
  ~Builtin();
};

} // namespace stone
#endif