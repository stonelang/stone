#ifndef STONE_ASTBUILTINCONTEXT_H
#define STONE_ASTBUILTINCONTEXT_H

#include "stone/AST/Types.h"

namespace stone {
namespace ast {
class ASTContext;

class Builtin final {
  ASTContext &sc;

public:
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

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
  Builtin(ASTContext &sc);
  ~Builtin();

private:
  void Initialize();

public:
  Type GetType(llvm::StringRef name);
};
} // namespace ast
} // namespace stone
#endif
