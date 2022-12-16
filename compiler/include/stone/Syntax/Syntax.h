#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Basic/STDTypeAlias.h"

#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {

class Syntax {

public:
  bool IsExpr() const;
  bool IsDecl() const;
  bool IsType() const;
  bool IsStmt() const;
};

} // namespace syn
} // namespace stone

#endif
