#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/StmtFactory.h"

using namespace stone;
using namespace stone::syn;

// SyntaxResult<Stmt *> Syntax::CreateIfStmt() { return StmtEmpty(); }

//== Statements ==/

BraceStmt::BraceStmt(SrcLoc lbLoc, llvm::ArrayRef<ASTNode> elements,
                     SrcLoc rbLoc)

    : Stmt(StmtKind::Brace), lbLoc(lbLoc), rbLoc(rbLoc) {

  // Bits.BraceStmt.NumElements = elts.size();

  std::uninitialized_copy(elements.begin(), elements.end(),
                          getTrailingObjects<ASTNode>());
}

BraceStmt *StmtFactory::MakeBraceStmt(SrcLoc lbloc,
                                      llvm::ArrayRef<ASTNode> elements,
                                      SrcLoc rbloc, ASTContext &sc,
                                      llvm::Optional<bool> implicit) {

  void *stmtPtr =
      sc.Allocate(BraceStmt::totalSizeToAlloc<ASTNode>(elements.size()),
                  alignof(BraceStmt));
  return ::new (stmtPtr) BraceStmt(lbloc, elements, rbloc);
}