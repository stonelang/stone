#ifndef STONE_SYNTAX_BUILTIN_H
#define STONE_SYNTAX_BUILTIN_H

#include "stone/Syntax/Type.h"

namespace stone {
namespace syn {

class SyntaxContext;

namespace builtin {

enum BuiltinID {};

class BuiltinProfile {
  const char *name;
  const char *ty;
  const char *attributes;
  const char *features;
};

} // namespace builtin

// enum class BuiltinTypeKind : std::underlying_type<TypeKind>::type {
// #define TYPE(id, parent)
// #define BUILTIN_TYPE(id, parent) \
//   id = std::underlying_type<TypeKind>::type(TypeKind::id),
// #include "stone/Syntax/TypeKind.def"
// };

/// Get the builtin type for the given name.
///
/// Returns a null type if the name is not a known builtin type name.
// Type GetBuiltinType(SyntaxContext &sc, llvm::StringRef name);

/// BuiltinValueKind - The set of (possibly overloaded) builtin functions.
enum class BuiltinID {
  None = 0,
#define BUILTIN(ID, Name, Attrs) ID,
#include "stone/Syntax/Builtin.def"
};

class Builtin final {
  SyntaxContext &sc;

public:
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

public:
  // const CanType VoidType;
  // const CanType BoolType;
  // const CanType NullType;

  // const CanType FloatTy;
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

public:
  Builtin(SyntaxContext &sc);
  ~Builtin();

public:
  Type GetType(llvm::StringRef name);
};
} // namespace syn
} // namespace stone
#endif
