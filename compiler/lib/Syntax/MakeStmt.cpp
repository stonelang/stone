#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

BraceStmt *Syntax::MakeBraceStmt(SrcLoc lbloc,
                                 llvm::ArrayRef<SyntaxNode> elements,
                                 SrcLoc rbloc, llvm::Optional<bool> implicit) {

  void *stmtPtr = GetSyntaxContext().Allocate(
      BraceStmt::totalSizeToAlloc<SyntaxNode>(elements.size()),
      alignof(BraceStmt));

  return ::new (stmtPtr) BraceStmt(lbloc, elements, rbloc);
}
