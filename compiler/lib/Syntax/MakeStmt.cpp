#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxFactory.h"

using namespace stone;
using namespace stone::syn;

//== Statements ==/
BraceStmt *syn::MakeBraceStmt(SrcLoc lbloc, llvm::ArrayRef<SyntaxNode> elements,
                              SrcLoc rbloc, SyntaxContext &sc,
                              llvm::Optional<bool> implicit) {

  void *stmtPtr =
      sc.Allocate(BraceStmt::totalSizeToAlloc<SyntaxNode>(elements.size()),
                  alignof(BraceStmt));

  return ::new (stmtPtr) BraceStmt(lbloc, elements, rbloc);
}