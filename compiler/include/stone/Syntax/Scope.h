#ifndef STONE_SYNTAX_SYNTAXSCOPE_H
#define STONE_SYNTAX_SYNTAXSCOPE_H

#include "stone/Basic/STDTypeAlias.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Syntax/Decl.h"
#include "stone/Syntax/SyntaxAllocation.h"

#include "llvm/ADT/SmallPtrSet.h"

namespace stone {
namespace syn {
class Decl;

// TODO: Think about
enum class ScopeKind : UInt8 {
  None = 0,
  /// A syntax file , which is the root of a scope.
  SyntaxFile,

  Decl,

  // Declaration of a type
  ForwardDecl,

  // Declaration of a type
  TypeDecl,

  /// A function/initializer/deinitializer.
  FunctionDecl,

  FunctionSignature,

  /// The parameters of a function/initializer/deinitializer.
  FunctionParams,

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
  Struct,

  /// This is a scope that corresponds to a block/closure object.
  /// Blocks serve as top-level scopes for some objects like labels, they
  /// also prevent things like break and continue.  BlockScopes always have
  /// the FunScope and DeclScope flags set as well.
  Block,

  /// This is a scope that corresponds to the
  /// template parameters of a C++ template. Template parameter
  /// scope starts at the 'template' keyword and ends when the
  /// template declaration ends.
  TemplateParams,

  /// This scope corresponds to an enum.
  Enum,

  /// This is a compound statement scope.
  BlockStmt,

  /// We are between inheritance colon and the real class/struct definition
  /// scope. TypeDeclaration?
  Interface,

  Constructor,

  Destructor,
};

class Scope final : public syn::SyntaxAllocation<Scope> {
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
} // namespace syn
} // namespace stone
#endif
