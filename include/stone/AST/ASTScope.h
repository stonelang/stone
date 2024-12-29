#ifndef STONE_AST_ASTSCOPE_H
#define STONE_AST_ASTSCOPE_H

#include "stone/AST/ASTAllocation.h"
#include "stone/AST/Decl.h"

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include <optional>

namespace stone {
class Decl;
enum class ASTScopeKind : uint8 {
  None = 0,
  File,
  TopLevelDecl,
  Decl,
  FunctionDecl,
  FunctionSignature,
  FunctionArguments,
  FunctionBody,
  EnumDecl,
  StructDecl,
  InterfaceDecl,
  ClassDecl,
};

class ASTScope : public ASTAllocation<ASTScope> {
  ASTScopeKind kind;
  DiagnosticEngine &diags;
  ASTScope *parent = nullptr;

  using DeclSet = llvm::SmallPtrSet<Decl *, 32>;
  DeclSet scopeDecls;

public:
  ASTScope(ASTScopeKind kind, DiagnosticEngine &diags,
           ASTScope *parent = nullptr);
  ~ASTScope();

  ASTScopeKind GetKind() { return kind; }
  ASTScope *GetParent() { return parent; }
  const char *GetName() { return ASTScope::GetName(GetKind()); }

  void AddDecl(Decl *d) { scopeDecls.insert(d); }
  void RemoveDecl(Decl *d) { scopeDecls.erase(d); }

protected:
  using Children = SmallVector<ASTScope *, 4>;
  /// Whether the given parent is the accessor node for an abstract
  /// storage declaration or is directly descended from it.

  /// The pointer:
  /// - Always set by the constructor, so that when creating a child
  ///   the parent chain is available. Null at the root.
  /// The int:
  /// - A flag indicating if the scope has been expanded yet or not.
  llvm::PointerIntPair<ASTScope *, 1> parentAndWasExpanded;

  /// Child scopes, sorted by source range.
  Children storedChildren;

  mutable std::optional<SrcRange> cachedCharSrcRange;

private:
  void Initialize();

public:
  static const char *GetName(ASTScopeKind kind);
};

} // namespace stone
#endif
