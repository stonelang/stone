#ifndef STONE_AST_DECLKIND_H
#define STONE_AST_DECLKIND_H

#include "stone/Basic/STDAlias.h"

namespace stone {

enum class DeclKind : UInt8 {
  None,
#define DECL(Id, Parent) Id,
#define LAST_DECL(Id) Count = Id,
#define DECL_RANGE(Id, FirstId, LastId)                                        \
  First##Id##Decl = FirstId, Last##Id##Decl = LastId,
#include "stone/AST/DeclKind.def"
};

enum class PrettyDeclKind : UInt8 {
  Import,
  EnumCase,
  IfConfig,
  Var,
  Param,
  Auto,
  Alias,
  Type,
  Enum,
  Fun,
  Struct,
  Interface,
  Constructor,
  Destructor,
  Module,
};

// llvm::StringRef GetDeclKindName(DeclKind kind);

} // namespace stone
#endif
