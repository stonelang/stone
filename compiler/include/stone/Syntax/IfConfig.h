#ifndef STONE_SYNTAX_IFCONFIG_H
#define STONE_SYNTAX_IFCONFIG_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
  
class Expr;
class SrcLoc;
struct SyntaxNode;

/// This represents one part of a #if block.  If the condition field is
/// non-null, then this represents a #if or a #elseif, otherwise it represents
/// an #else block.
struct IfConfig {
  /// The location of the #if, #elseif, or #else keyword.
  SourceLoc Loc;

  /// The condition guarding this #if or #elseif block.  If this is null, this
  /// is a #else clause.
  Expr *cond;

  /// Elements inside the clause
  llvm::ArrayRef<SyntaxNode> elements;

  /// True if this is the active clause of the #if block.
  const bool isActive;

  IfConfig(SrcLoc loc, Expr *cond, llvm::ArrayRef<SyntaxNode> elements,
           bool isActive)
      : loc(loc), cond(cond), elements(Elements), isActive(isActive) {}
};

} // end namespace stone

#endif
