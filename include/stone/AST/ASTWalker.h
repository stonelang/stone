#ifndef STONE_AST_ASTWALKER_H
#define STONE_AST_ASTWALKER_H

#include "stone/AST/ASTNode.h"

#include "llvm/ADT/PointerUnion.h"

#include <optional>
#include <utility>

namespace stone {

class Decl;
class Stmt;
class Expr;
class Module;
class Type;
class ASTWalker;

enum class SemanticRefKind : uint8_t {
  ModuleRef = 0,
  DeclRef,
  DeclMemberRef,
  DeclConstructorRef,
  TypeRef,
  EnumElementRef,
  SubscriptRef,     // TODO: UB
  DynamicMemberRef, // TODO: UB
};

/// An abstract class used to traverse an AST.
class ASTWalker {
public:
  class ParentASTNode {
    ASTDescription parentKind;
    void *Ptr = nullptr;

  public:
    ParentASTNode(Module *M) : parentKind(ASTDescription::Module), Ptr(M) {}
    ParentASTNode(Decl *D) : parentKind(ASTDescription::Decl), Ptr(D) {}
    ParentASTNode(Stmt *S) : parentKind(ASTDescription::Stmt), Ptr(S) {}
    ParentASTNode(Expr *E) : parentKind(ASTDescription::Expr), Ptr(E) {}
    ParentASTNode(Type *Q) : parentKind(ASTDescription::Type), Ptr(Q) {}

    bool IsNull() const { return Ptr == nullptr; }

    ASTDescription GetParentKind() const {
      assert(!IsNull());
      return parentKind;
    }

    Module *CastToModule() const {
      return (parentKind == ASTDescription::Module) ? static_cast<Module *>(Ptr)
                                                    : nullptr;
    }
    Decl *CastToDecl() const {
      return (parentKind == ASTDescription::Decl) ? static_cast<Decl *>(Ptr)
                                                  : nullptr;
    }
    Stmt *CastToStmt() const {
      return (parentKind == ASTDescription::Stmt) ? static_cast<Stmt *>(Ptr)
                                                  : nullptr;
    }
    Expr *CastToExpr() const {
      return parentKind == ASTDescription::Expr ? static_cast<Expr *>(Ptr)
                                                : nullptr;
    }
    Type *CastToType() const {
      return (parentKind == ASTDescription::Type) ? static_cast<Type *>(Ptr)
                                                  : nullptr;
    }
  };

  /// The parent of the node we are visiting.
  ParentASTNode parent;

  /// This method is called when first visiting an expression
  /// before walking into its children.
  ///
  /// \param E The expression to check.
  ///
  /// \returns a pair indicating whether to visit the children along with
  /// the expression that should replace this expression in the tree. If the
  /// latter is null, the traversal will be terminated.
  ///
  /// The default implementation returns \c {true, E}.
  virtual std::pair<bool, Expr *> WalkToExprPre(Expr *E) { return {true, E}; }

  /// This method is called after visiting an expression's children.
  /// If it returns null, the walk is terminated; otherwise, the
  /// returned expression is spliced in where the old expression
  /// previously appeared.
  ///
  /// The default implementation always returns its argument.
  virtual Expr *WalkToExprPost(Expr *E) { return E; }

  /// This method is called when first visiting a statement before
  /// walking into its children.
  ///
  /// \param S The statement to check.
  ///
  /// \returns a pair indicating whether to visit the children along with
  /// the statement that should replace this statement in the tree. If the
  /// latter is null, the traversal will be terminated.
  ///
  /// The default implementation returns \c {true, S}.
  virtual std::pair<bool, Stmt *> WalkToStmtPre(Stmt *S) { return {true, S}; }

  /// This method is called after visiting a statement's children.  If
  /// it returns null, the walk is terminated; otherwise, the returned
  /// statement is spliced in where the old statement previously
  /// appeared.
  ///
  /// The default implementation always returns its argument.
  virtual Stmt *WalkToStmtPost(Stmt *S) { return S; }

  // /// This method is called when first visiting a pattern before walking into
  // /// its children.
  // ///
  // /// \param P The statement to check.
  // ///
  // /// \returns a pair indicating whether to visit the children along with
  // /// the statement that should replace this statement in the tree. If the
  // /// latter is null, the traversal will be terminated.
  // ///
  // /// The default implementation returns \c {true, P}.
  // virtual std::pair<bool, Pattern*> WalkToPatternPre(Pattern *P) {
  //   return { true, P };
  // }

  // /// This method is called after visiting a pattern's children.  If
  // /// it returns null, the walk is terminated; otherwise, the returned
  // /// pattern is spliced in where the old statement previously
  // /// appeared.
  // ///
  // /// The default implementation always returns its argument.
  // virtual Pattern *walkToPatternPost(Pattern *P) { return P; }

  /// walkToDeclPre - This method is called when first visiting a decl, before
  /// walking into its children.  If it returns false, the subtree is skipped.
  ///
  /// \param D The declaration to check. The callee may update this declaration
  /// in-place.
  virtual bool WalkToDeclPre(Decl *D) { return true; }

  /// walkToDeclPost - This method is called after visiting the children of a
  /// decl.  If it returns false, the remaining traversal is terminated and
  /// returns failure.
  virtual bool WalkToDeclPost(Decl *D) { return true; }

  /// This method is called when first visiting a TypeRepr, before
  /// walking into its children.  If it returns false, the subtree is skipped.
  ///
  /// \param T The TypeRepr to check.
  virtual bool WalkToTypePre(Type *T) { return true; }

  /// This method is called after visiting the children of a TypeRepr.
  /// If it returns false, the remaining traversal is terminated and returns
  /// failure.
  virtual bool WalkToTypePost(Type *T) { return true; }

  // /// This method configures whether the walker should explore into the
  // generic
  // /// params in AbstractFunctionDecl and NominalTypeDecl.
  // virtual bool shouldWalkIntoTemplateParams() { return false; }

  // /// This method configures whether the walker should walk into the
  // /// initializers of lazy variables.  These initializers are semantically
  // /// different from other initializers in their context and so sometimes
  // /// should not be visited.
  // ///
  // /// Note that visiting the body of the lazy getter will find a
  // /// LazyInitializerExpr with the initializer as its sub-expression.
  // /// However, ASTWalker does not walk into LazyInitializerExprs on its
  // own. virtual bool shouldWalkIntoLazyInitializers() { return true; }

  /// This method configures whether the walker should visit the body of a
  /// closure that was checked separately from its enclosing expression.
  ///
  /// For work that is performed for every top-level expression, this should
  /// be overridden to return false, to avoid duplicating work or visiting
  /// bodies of closures that have not yet been type checked.
  // virtual bool shouldWalkIntoSeparatelyCheckedClosure(ClosureExpr *) {
  //   return true;
  // }

  // /// This method configures whether the walker should visit the body of a
  // /// TapExpr.
  // virtual bool shouldWalkIntoTapExpression() { return true; }

  // /// This method configures whether the the walker should visit the
  // underlying
  // /// value of a property wrapper placeholder.
  // virtual bool shouldWalkIntoPropertyWrapperPlaceholderValue() { return true;
  // }

  // /// This method configures whether the walker should visit the capture
  // /// initializer expressions within a capture list directly, rather than
  // /// walking the declarations.
  // virtual bool shouldWalkCaptureInitializerExpressions() { return false; }

  /// This method configures whether the walker should exhibit the legacy
  /// behavior where accessors appear as peers of their storage, rather
  /// than children nested inside of it.
  ///
  /// Please don't write new ASTWalker implementations that override this
  /// method to return true; instead, refactor existing code as needed
  /// until eventually we can remove this altogether.
  // virtual bool shouldWalkAccessorsTheOldWay() { return false; }

  /// walkToParameterListPre - This method is called when first visiting a
  /// ParameterList, before walking into its parameters.  If it returns false,
  /// the subtree is skipped.
  ///
  // virtual bool walkToParameterListPre(ParameterList *PL) { return true; }

  /// walkToParameterListPost - This method is called after visiting the
  /// children of a parameter list.  If it returns false, the remaining
  /// traversal is terminated and returns failure.
  // virtual bool walkToParameterListPost(ParameterList *PL) { return true; }

  /// This method is called when first visiting an argument list before walking
  /// into its arguments.
  ///
  /// \param ArgList The argument list to walk.
  ///
  /// \returns a pair indicating whether to visit the arguments, along with
  /// the argument list that should replace this argument list in the tree. If
  /// the latter is null, the traversal will be terminated.
  ///
  /// The default implementation returns \c {true, ArgList}.
  // virtual std::pair<bool, ArgumentList *>
  // walkToArgumentListPre(ArgumentList *ArgList) {
  //   return {true, ArgList};
  // }

  /// This method is called after visiting the arguments in an argument list.
  /// If it returns null, the walk is terminated; otherwise, the
  /// returned argument list is spliced in where the old argument list
  /// previously appeared.
  ///
  /// The default implementation always returns the argument list.
  // virtual ArgumentList *walkToArgumentListPost(ArgumentList *ArgList) {
  //   return ArgList;
  // }

protected:
  ASTWalker() = default;
  ASTWalker(const ASTWalker &) = default;
  virtual ~ASTWalker() = default;
};

} // namespace stone
#endif
