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
  SrcLoc loc;
  Type *tyPtr = nullptr;
  //TypeModifierList syntacticModifiers;

public:

  explicit TypeResult(TypeResultKind kind, SrcLoc loc, Type* tyPtr = nullptr)
};

} // namespace stone
#endif
