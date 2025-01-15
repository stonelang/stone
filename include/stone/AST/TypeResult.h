#ifndef STONE_AST_TYPERESULT_H
#define STONE_AST_TYPERESULT_H

#include "stone/Basic/SrcLoc.h"

namespace stone {

enum class TypeResultKind : uint8_t {
#define TYPEREPR(ID, PARENT) ID,
#define LAST_TYPEREPR(ID) Last_TypeResult = ID,
#include "TypeResultNode.def"
};

class TypeResult {
  TypeResultKind kind;
  SrcLoc loc;
  UnderlyingType *underlyingType;
  // TypeModifierList syntacticModifiers;

public:
  explicit TypeResult(TypeResultKind kind, SrcLoc loc,
                      UnderlyingType *underlyingType)
      : kind(kind), loc(loc), underlyingType(underlyingType) {}
};

} // namespace stone
#endif
