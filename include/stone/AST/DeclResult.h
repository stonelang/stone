#ifndef STONE_AST_TYPERESULT_H
#define STONE_AST_TYPERESULT_H

namespace stone {

enum class TypeResultKind : uint8_t {
#define TYPEREPR(ID, PARENT) ID,
#define LAST_TYPEREPR(ID) Last_TypeResult = ID,
#include "TypeResultNode.def"
};

class TypeResult final {};

} // namespace stone
#endif
