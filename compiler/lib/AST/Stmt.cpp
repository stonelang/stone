#include "stone/AST/Stmt.h"
#include "stone/AST/ASTContext.h"

using namespace stone;
// ParserResult<Stmt *> Syntax::CreateIfStmt() { return StmtEmpty(); }

//== Statements ==/

BraceStmt::BraceStmt(SrcLoc lbLoc, llvm::ArrayRef<ASTNode> elements,
                     SrcLoc rbLoc)

    : Stmt(StmtKind::Brace), lbLoc(lbLoc), rbLoc(rbLoc) {

  // Bits.BraceStmt.NumElements = elts.size();

  std::uninitialized_copy(elements.begin(), elements.end(),
                          getTrailingObjects<ASTNode>());
}

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
