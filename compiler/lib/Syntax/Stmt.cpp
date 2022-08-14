#include "stone/Syntax/Stmt.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

// SyntaxResult<Stmt *> Syntax::CreateIfStmt() { return StmtEmpty(); }

BraceStmt::BraceStmt(SrcLoc lbLoc, llvm::ArrayRef<SyntaxNode> elements,
                     SrcLoc rbLoc)

    : Stmt(StmtKind::Brace), lbLoc(lbLoc), rbLoc(rbLoc) {

  // Bits.BraceStmt.NumElements = elts.size();

  std::uninitialized_copy(elements.begin(), elements.end(),
                          getTrailingObjects<SyntaxNode>());
}