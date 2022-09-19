#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"

using namespace stone;
using namespace stone::syn;

// SyntaxResult<Stmt *> Syntax::CreateIfStmt() { return StmtEmpty(); }

//== Statements ==/

BraceStmt::BraceStmt(SrcLoc lbLoc, llvm::ArrayRef<SyntaxNode> elements,
                     SrcLoc rbLoc)

    : Stmt(StmtKind::Brace), lbLoc(lbLoc), rbLoc(rbLoc) {

  // Bits.BraceStmt.NumElements = elts.size();

  std::uninitialized_copy(elements.begin(), elements.end(),
                          getTrailingObjects<SyntaxNode>());
}

BraceStmt *BraceStmtFactory::Create(SrcLoc lbloc,
                                    llvm::ArrayRef<SyntaxNode> elements,
                                    SrcLoc rbloc, SyntaxContext &sc,
                                    llvm::Optional<bool> implicit) {

  void *stmtPtr =
      sc.Allocate(BraceStmt::totalSizeToAlloc<SyntaxNode>(elements.size()),
                  alignof(BraceStmt));
  return ::new (stmtPtr) BraceStmt(lbloc, elements, rbloc);
}