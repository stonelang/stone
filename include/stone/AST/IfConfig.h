#ifndef STONE_AST_IFCONFIG_H
#define STONE_AST_IFCONFIG_H

#include "stone/Basic/SrcLoc.h"
#include "llvm/ADT/ArrayRef.h"

namespace stone {
class SrcLoc;

class Expr;
struct ASTNode;

/// This represents one part of a #if block.  If the condition field is
/// non-null, then this represents a #if or a #elseif, otherwise it represents
/// an #else block.
struct IfConfigClause {
  /// The location of the #if, #elseif, or #else keyword.
  SrcLoc loc;

  /// The condition guarding this #if or #elseif block.  If this is null, this
  /// is a #else clause.
  Expr *cond;

  /// Elements inside the clause
  llvm::ArrayRef<ASTNode> elements;

  /// True if this is the active clause of the #if block.
  const bool isActive;

  IfConfigClause(SrcLoc loc, Expr *cond, llvm::ArrayRef<ASTNode> elements,
                 bool isActive)
      : loc(loc), cond(cond), elements(elements), isActive(isActive) {}
};

} // end namespace stone

#endif
