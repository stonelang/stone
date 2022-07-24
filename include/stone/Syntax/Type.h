#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

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

// QualifierType
class QualType {
public:
  QualType() = default;
};

class FunctionTypeBase : public Type {};

class FunctionType : public FunctionTypeBase {
  // The type returned by the function.
  QualType returnType;

public:
  QualType GetReturnType() { return returnType; }
};

} // namespace syn
} // namespace stone
#endif
