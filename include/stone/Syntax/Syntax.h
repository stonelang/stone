#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Syntax/Type.h"

namespace stone {
class InflightDiagnostic;

namespace syn {
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class MatchStmt;
class Expr;
class Syntax;
class SyntaxFile;

class Syntax final {
  // Verifier verifier;
  TreeContext &tc;

public:
  Syntax(const Syntax &) = delete;
  Syntax(Syntax &&) = delete;
  Syntax &operator=(const Syntax &) = delete;
  Syntax &operator=(Syntax &&) = delete;
  Syntax() = delete;

public:
  Syntax(TreeContext &tc);
  ~Syntax();

public:
  // Verifier &GetVerifier() { return verifier; }
  TreeContext &GetTreeContext() { return tc; }

public:
  Module *MakeModuleDecl(Identifier &name, bool isMainModule);

public:
  void VerifyDecl(Decl *d);

public:
  FunDecl *MakeFunDecl(SrcLoc loc, DeclContext *dc);
  void VerifyFunDecl(Decl *d);

public:
  StructDecl *MakeStructDecl(SrcLoc loc, DeclContext *dc);
  void VerifyStructDecl(Decl *d);

public:
  bool HasError() { return tc.GetBasic().HasError(); }
  Basic &GetBasic() { return tc.GetBasic(); }

public:
  Identifier &MakeIdentifier(llvm::StringRef name);
  DeclName MakeDeclName();

public:
  /// \param extraSpace The amount of extra space to allocate after the object
  /// -- generally for clang nodes.
  template <typename DeclTy, typename AllocatorTy>
  static void *AllocateDeclMem(AllocatorTy &allocatorTy, size_t baseSize,
                               bool extraSpace = false);
};

} // namespace syn
} // namespace stone
#endif
