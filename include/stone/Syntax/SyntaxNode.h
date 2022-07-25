#ifndef STONE_SYNTAX_SYNTAXNODE_H
#define STONE_SYNTAX_SYNTAXNODE_H


#include "llvm/ADT/PointerUnion.h"

namespace stone {
class SrcLoc;
class SrcRange;

namespace syn {

class Expr;
class Stmt;
class Decl;

class SyntaxWalker;

enum class ExprKind : uint8_t;
enum class DeclKind : uint8_t;
enum class StmtKind : uint8_t; // TODO: May not want uint8_t

struct SyntaxNode : public llvm::PointerUnion<Expr *, Stmt *, Decl *> {
  // Inherit the constructors from PointerUnion.
  using PointerUnion::PointerUnion;

  SrcRange GetSrcRange() const;

  /// Return the location of the start of the statement.
  SrcLoc GetStartLoc() const;

  /// Return the location of the end of the statement.
  SrcLoc GetEndLoc() const;

  void Walk(SyntaxWalker &walker);
  void Walk(SyntaxWalker &&walker) { Walk(walker); }
};

} // namespace syn
} // namespace stone

// namespace llvm {
// using stone::syn::SyntaxNode;
// template <> struct DenseMapInfo<SyntaxNode> {
//   static inline SyntaxNode getEmptyKey() {
//     return DenseMapInfo<stone::syn::Expr *>::getEmptyKey();
//   }
//   static inline SyntaxNode getTombstoneKey() {
//     return DenseMapInfo<stone::syn::Expr *>::getTombstoneKey();
//   }
//   static unsigned getHashValue(const SyntaxNode Val) {
//     return DenseMapInfo<void *>::getHashValue(Val.getOpaqueValue());
//   }
//   static bool isEqual(const SyntaxNode LHS, const SyntaxNode RHS) {
//     return LHS.getOpaqueValue() == RHS.getOpaqueValue();
//   }
// };
// } // namespace llvm

#endif
