#ifndef STONE_AST_NODE_H
#define STONE_AST_NODE_H

#include "stone/Basic/STDAlias.h"

#include "llvm/ADT/PointerUnion.h"

namespace stone {
class SrcLoc;
class SrcRange;

namespace syn {

class Expr;
class Stmt;
class Decl;
class Type;
class DeclContext;
class ASTWalker;

enum class ExprKind : uint8_t;
enum class DeclKind : uint8_t;
enum class StmtKind : uint8_t; // TODO: May not want uint8_t

enum class ASTNodeStatus : UInt8 {
  None,
  Created,
  Parsed,
  TypeChecked,
};

enum class ASTDescription : uint8_t {
  Module,
  Decl,
  Stmt,
  Expr,
  QualType,
};

struct ASTNode : public llvm::PointerUnion<Expr *, Stmt *, Decl *, Type *> {
  // Inherit the constructors from PointerUnion.
  using PointerUnion::PointerUnion;

  SrcRange GetSrcRange() const;

  /// Return the location of the start of the statement.
  SrcLoc GetStartLoc() const;

  /// Return the location of the end of the statement.
  SrcLoc GetEndLoc() const;

  ASTNodeStatus Status;

  void Walk(ASTWalker &walker);
  void Walk(ASTWalker &&walker) { Walk(walker); }
};

} // namespace syn
} // namespace stone

// namespace llvm {
// using stone::syn::ASTNode;
// template <> struct DenseMapInfo<ASTNode> {
//   static inline ASTNode getEmptyKey() {
//     return DenseMapInfo<stone::syn::Expr *>::getEmptyKey();
//   }
//   static inline ASTNode getTombstoneKey() {
//     return DenseMapInfo<stone::syn::Expr *>::getTombstoneKey();
//   }
//   static unsigned getHashValue(const ASTNode Val) {
//     return DenseMapInfo<void *>::getHashValue(Val.getOpaqueValue());
//   }
//   static bool isEqual(const ASTNode LHS, const ASTNode RHS) {
//     return LHS.getOpaqueValue() == RHS.getOpaqueValue();
//   }
// };
// } // namespace llvm

#endif
