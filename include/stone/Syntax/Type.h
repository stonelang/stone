#ifndef STONE_SYNTAX_TYPE_H
#define STONE_SYNTAX_TYPE_H

#include <string>

namespace stone {
namespace syn {

enum class TypeKind : uint8_t {
#define TYPE(id, parent) id,
#define LAST_TYPE(id) Last_Type = id,
#define TYPE_RANGE(Id, FirstId, LastId)                                        \
  First_##Id##Type = FirstId, Last_##Id##Type = LastId,
#include "stone/Syntax/TypeKind.def"
};

class TypeContext final {
public:
};

class ExtQuals;
class QualType;
// class ConceptDecl;
class StructDecl;
class Type;

class TypeLoc {};

class Type {
public:
};

class QualType {
public:
  QualType() = default;
};

} // namespace syn
} // namespace stone
#endif
