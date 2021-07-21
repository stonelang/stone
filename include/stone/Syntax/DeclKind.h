#ifndef STONE_SYNTAX_DECLKIND_H
#define STONE_SYNTAX_DECLKIND_H

namespace stone {
namespace syn {

enum class DeclKind : uint8_t{
  None,
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) LastDecl = Id,
#define DECL_RANGE(Id, FirstId, LastId)                                        \
  First##Id##Decl = FirstId, Last##Id##Decl = LastId,
#include "stone/Syntax/DeclKind.def"
};
} // namespace syn

} // namespace stone
#endif
