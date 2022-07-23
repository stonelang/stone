#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"
#include "stone/Syntax/TypeLoc.h"

#include <string>

namespace stone {
namespace syn {

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

class FunctionType : public Type {
  // The type returned by the function.
  QualType returnType;

public:
};

} // namespace syn
} // namespace stone
#endif
