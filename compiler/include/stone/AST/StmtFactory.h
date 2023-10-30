#ifndef STONE_ASTSTMTFACTORY_H
#define STONE_ASTSTMTFACTORY_H

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace ast {
class Stmt;
class IfStmt;
class SwitchStmt;
class ASTNode;

} // namespace ast

namespace ast {

struct StmtFactory final {
  static BraceStmt *MakeBraceStmt(SrcLoc lbloc,
                                  llvm::ArrayRef<ASTNode> elements,
                                  SrcLoc rbloc, ASTContext &sc,
                                  llvm::Optional<bool> implicit = llvm::None);
};

} // namespace ast
} // namespace stone

#endif
