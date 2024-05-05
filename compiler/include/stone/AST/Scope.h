#ifndef STONE_AST_ASTSCOPE_H
#define STONE_AST_ASTSCOPE_H

#include "stone/AST/Decl.h"
#include "stone/Basic/Memory.h"
#include "stone/Basic/STDAlias.h"

#include "llvm/ADT/SmallPtrSet.h"

namespace stone {
class Decl;

// TODO: Think about
enum class ScopeKind : UInt8 {
  None = 0,

  TopLevel,
  /// A syntax file , which is the root of a scope.
  SourceFile,

  Decl,

  ImportDecl,

  // Declaration of a type
  TypeDecl,

  /// A function/initializer/deinitializer.
  FunDecl,

  FunctionSignature,

  /// The parameters of a function/initializer/deinitializer.
  FunctionArguments,

  /// The body of a function.
  FunctionBody,

  /// Scope for a closure.
  Closure,
  /// This is a while, do, switch, for, etc that can have break
  /// statements embedded into it.
  Break,

  /// This is a while, do, for, which can have continue statements
  /// embedded into it.
  Continue,

  /// The controlling scope in a if/switch/while/for statement.
  Control,

  /// The scope of a struct/union/class definition.
  StructDecl,
  /// This is a scope that corresponds to a block/closure object.
  /// Blocks serve as top-level scopes for some objects like labels, they
  /// also prevent things like break and continue.  BlockScopes always have
  /// the FunScope and DeclScope flags set as well.
  Block,
  /// This is a scope that corresponds to the
  /// template parameters of a C++ template. Template parameter
  /// scope starts at the 'template' keyword and ends when the
  /// template declaration ends.
  TemplateArguments,

  /// This scope corresponds to an enum.
  EnumDecl,

  /// This is a compound statement scope.
  BraceStmt,
  /// We are between inheritance colon and the real class/struct definition
  /// scope. TypeDeclaration?
  InterfaceDecl,

  ///
  ConstructorDecl,

  ///
  DestructorDecl,

  ///
  ReturnClause,

  ///
  AutoDecl,

  ///
  VarDecl,

  ///
  NewDecl,
  ///
  AliasDecl,
  ///
  Type,

  FunctionType,

};

class Scope final : public MemoryAllocation<Scope> {
  ScopeKind kind;
  DiagnosticEngine &diags;
  Scope *parent = nullptr;

  using DeclSet = llvm::SmallPtrSet<Decl *, 32>;
  DeclSet scopeDecls;

public:
  Scope(ScopeKind kind, DiagnosticEngine &diags, Scope *parent = nullptr);
  ~Scope();

  ScopeKind GetKind() { return kind; }
  Scope *GetParent() { return parent; }
  const char *GetName() { return Scope::GetName(GetKind()); }

  void AddDecl(Decl *d) { scopeDecls.insert(d); }
  void RemoveDecl(Decl *d) { scopeDecls.erase(d); }

private:
  void Initialize();

public:
  static const char *GetName(ScopeKind kind);
};

} // namespace stone
#endif
