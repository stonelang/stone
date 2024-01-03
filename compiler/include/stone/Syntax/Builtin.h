#ifndef STONE_SYNTAX_BUILTINCONTEXT_H
#define STONE_SYNTAX_BUILTINCONTEXT_H

#include "stone/Syntax/Identifier.h"
#include "stone/Syntax/Types.h"

namespace stone {

class ASTContext;

class BuiltinIdentifierCache final {
  ASTContext &astContext;

public:
  BuiltinIdentifierCache(const BuiltinIdentifierCache &) = delete;
  void operator=(const BuiltinIdentifierCache &) = delete;

public:
  // Declare the set of builtin identifiers.
#define BUILTIN_IDENTIFIER_WITH_NAME(Name, IdStr)                              \
  Identifier Builtin##Name##Identifier;
#include "stone/Syntax/BuiltinIdentifiers.def"

public:
  BuiltinIdentifierCache(ASTContext &astContext);
};

class BuiltinFunctionCache final {
  ASTContext &astContext;

public:
  BuiltinFunctionCache(const BuiltinFunctionCache &) = delete;
  void operator=(const BuiltinFunctionCache &) = delete;

public:
  BuiltinFunctionCache(ASTContext &astContext);
};

class BuiltinDeclCache final {
  ASTContext &astContext;

public:
  BuiltinDeclCache(const BuiltinDeclCache &) = delete;
  void operator=(const BuiltinDeclCache &) = delete;

public:
  BuiltinDeclCache(ASTContext &astContext);
};

class BuiltinTypeCache final {
  ASTContext &astContext;

public:
  BuiltinTypeCache(const BuiltinTypeCache &) = delete;
  void operator=(const BuiltinTypeCache &) = delete;

public:
  BuiltinTypeCache(ASTContext &astContext);

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

public:
  Type GetType(llvm::StringRef name);
};

class Builtin final {
  ASTContext &astContext;
  BuiltinTypeCache typeCache;
  BuiltinIdentifierCache identifierCache;
  BuiltinFunctionCache functionCache;
  BuiltinDeclCache declCache;

public:
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

public:
  Builtin(ASTContext &astContext);
  ~Builtin();

public:
  BuiltinTypeCache &GetTypeCache() { return typeCache; }
  BuiltinIdentifierCache &GetIdentifierCache() { return identifierCache; }
  BuiltinFunctionCache &GetFunctionCache() { return functionCache; }
  BuiltinDeclCache &GetDeclCache() { return declCache; }
};

} // namespace stone
#endif
