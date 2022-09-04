#ifndef STONE_SYNTAX_BUILTIN_H
#define STONE_SYNTAX_BUILTIN_H

#include "stone/Syntax/Types.h"

namespace stone {
namespace syn {
class Type;
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
public:
  Builtin(const Builtin &) = delete;
  void operator=(const Builtin &) = delete;

public:
  void InitType(SyntaxContext &tc);
  void InitTypes(SyntaxContext &tc);

public:
  Builtin() = default;
  ~Builtin();

  void Init(SyntaxContext &tc);

public:
  Type GetType(llvm::StringRef name);
};
} // namespace syn
} // namespace stone
#endif
