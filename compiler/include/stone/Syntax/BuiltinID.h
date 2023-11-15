#ifndef STONE_SYNTAX_BUILTINID_H
#define STONE_SYNTAX_BUILTINID_H

namespace stone {
namespace syn {

// enum class BuiltinTypeKind : std::underlying_type<TypeKind>::type {
// #define TYPE(id, parent)
// #define BUILTIN_TYPE(id, parent) \
//   id = std::underlying_type<TypeKind>::type(TypeKind::id),
// #include "stone/Syntax/TypeKind.def"
// };

/// Get the builtin type for the given name.
///
/// Returns a null type if the name is not a known builtin type name.
// Type GetBuiltinType(ASTContext &sc, llvm::StringRef name);

/// BuiltinValueKind - The set of (possibly overloaded) builtin functions.
enum class BuiltinID {
  None = 0,
#define BUILTIN(ID, Name, Attrs) ID,
#include "stone/Syntax/Builtin.def"
};

class BuiltinProfile {
  const char *name;
  const char *ty;
  const char *attributes;
  const char *features;
};

} // namespace syn
} // namespace stone
#endif
