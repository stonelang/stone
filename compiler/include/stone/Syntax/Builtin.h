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
  // const CanQualType VoidType;
  //   const CanQualType BoolType;

  // const CanQualType FloatTy;
  const CanQualType Float16Type; /// 32-bit IEEE floating point
  const CanQualType Float32Type; /// 32-bit IEEE floating point
  const CanQualType Float64Type; /// 64-bit IEEE floating point
  // const CanQualType Float128Type; /// 128-bit IEEE floating point

  // const CanQualType Int8Type;
  const CanQualType Int16Type;
  const CanQualType Int32Type;
  const CanQualType Int64Type;
  // const CanQualType Int128Type;

  // const CanQualType UInt8Type;
  // const CanQualType UInt16Type;
  // const CanQualType UInt32Type;
  // const CanQualType UInt64Type;
  // const CanQualType UInt128Type;

public:
  Builtin(SyntaxContext &sc);
  ~Builtin();

public:
  Type GetType(llvm::StringRef name);
};
} // namespace syn
} // namespace stone
#endif
