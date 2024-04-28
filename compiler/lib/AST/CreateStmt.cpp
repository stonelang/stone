#include "stone/AST/ASTContext.h"
#include "stone/AST/Stmt.h"

using namespace stone;

BraceStmt *BraceStmt::Create(SrcLoc lbloc, llvm::ArrayRef<ASTNode> elements,
                             SrcLoc rbloc, ASTContext &astContext,
                             std::optional<bool> implicit) {

  void *stmtPtr = astContext.AllocateMemory(
      BraceStmt::totalSizeToAlloc<ASTNode>(elements.size()),
      alignof(BraceStmt));
  return ::new (stmtPtr) BraceStmt(lbloc, elements, rbloc);
}
