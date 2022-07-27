#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"
#include "stone/Syntax/TypeLoc.h"

#include <string>

namespace stone {
namespace syn {

enum class GC : uint8_t { None = 0, Weak, Strong };

enum class TypeQualifer : uint8_t {
  None = 0,
  Const = 1,
  Restrict = 2,
  Volatile = 4,
  Unaligned = 8,
  // This has no corresponding Qualifiers::TQ value, because it's not treated
  // as a qualifier in our type system.
  Atomic = 16
};

// Extended Qualifiers
class ExtQuals;
// Qualified Types

class QualType;
// class ConceptDecl;
class StructDecl;
class Type;

class TypeContext final {
public:
};

class alignas(1 << TypeAlignInBits) Type
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {};

// TODO: QualType to TypeRep
class alignas(1 << QualTypeAlignInBits) QualType
    : public SyntaxAllocation<QualType> {
public:
  QualType() = default;
  QualType(const Type *ty, unsigned quals) {}

public:
  /// Retrieves a pointer to the underlying (unqualified) type.
  ///
  /// This function requires that the type not be NULL. If the type might be
  /// NULL, use the (slightly less efficient) \c getTypePtrOrNull().
  const Type *GetTypePtr() const;

  const Type *GetTypePtrOrNull() const;
};

class FunctionTypeBase : public Type {};

class FunctionType : public FunctionTypeBase {
  // The type returned by the function.
  QualType returnType;

public:
  QualType GetReturnType() { return returnType; }
};

using TypeRep = OpaquePtr<QualType>;
using UnionTypeRep = UnionOpaquePtr<QualType>;

} // namespace syn
} // namespace stone
#endif
