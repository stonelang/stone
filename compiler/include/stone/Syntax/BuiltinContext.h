#ifndef STONE_SYNTAX_BUILTINCONTEXT_H
#define STONE_SYNTAX_BUILTINCONTEXT_H

#include "stone/Syntax/Types.h"

namespace stone {

class ASTContext;

class BuiltinContext final {
  ASTContext &sc;

public:
  BuiltinContext(const BuiltinContext &) = delete;
  void operator=(const BuiltinContext &) = delete;

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

  const CanType BuiltinVoidType;
  const CanType BuiltinNullType;
  const CanType BuiltinBoolType;

public:
  BuiltinContext(ASTContext &sc);
  ~BuiltinContext();

private:
  void Initialize();

public:
  Type GetType(llvm::StringRef name);
};

} // namespace stone
#endif
