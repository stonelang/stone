#ifndef STONE_SYNTAX_STMTFACTORY_H
#define STONE_SYNTAX_STMTFACTORY_H

#include "llvm/ADT/None.h"
#include "llvm/ADT/PointerUnion.h"

namespace stone {
namespace syn {
class Stmt;
class IfStmt;
class SwitchStmt;
class ASTNode;

} // namespace syn

namespace syn {

struct StmtFactory final {
  static BraceStmt *MakeBraceStmt(SrcLoc lbloc,
                                  llvm::ArrayRef<ASTNode> elements,
                                  SrcLoc rbloc, ASTContext &sc,
                                  llvm::Optional<bool> implicit = llvm::None);
};

} // namespace syn
} // namespace stone

#endif
