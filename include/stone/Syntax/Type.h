#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include "stone/Syntax/SyntaxAllocation.h"
#include "stone/Syntax/TypeAlignment.h"
#include "stone/Syntax/TypeKind.h"

#include <string>

namespace stone {
namespace syn {

class TypeContext final {
public:
};

// Extended Qualifiers
class ExtQuals;

class QualType;

// class ConceptDecl;
class StructDecl;
class Type;

class TypeLoc {};

class alignas(1 << TypeAlignInBits) Type
    : public SyntaxAllocation<std::aligned_storage<8, 8>::type> {};


// QualifierType
class QualType {
public:
  QualType() = default;
};

} // namespace syn
} // namespace stone
#endif
