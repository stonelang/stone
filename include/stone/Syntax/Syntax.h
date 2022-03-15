#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Core/DiagnosticEngine.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/TreeContext.h"
#include "stone/Syntax/Type.h"

namespace stone {
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

public:
  bool HasError() { return tc.GetContext().HasError(); }
  Context &GetContext() { return tc.GetContext(); }

public:
  Identifier &MakeIdentifier(llvm::StringRef name);
  DeclName MakeDeclName();

public:
  stone::InFlightDiagnostic DiagnoseSyntax(SrcLoc loc, DiagID diagID) {
    return tc.GetContext().GetDiagEngine().Printd(
        loc, SyntaxDiagnostic(
                 DiagnosticContext(diagID, llvm::ArrayRef<diag::Argument>())));
  }
  stone::InFlightDiagnostic
  DiagnoseSyntax(SrcLoc loc, DiagID diagID,
                 llvm::ArrayRef<diag::Argument> args) {
    return tc.GetContext().GetDiagEngine().Printd(
        loc, SyntaxDiagnostic(DiagnosticContext(diagID, args)));
  }

  template <typename... ArgTypes>
  stone::InFlightDiagnostic
  DiagnoseSyntax(SrcLoc loc, Diag<ArgTypes...> id,
                 typename stone::detail::PassArgument<ArgTypes>::type... args) {

    return tc.GetContext().GetDiagEngine().Printd(
        loc, SyntaxDiagnostic(DiagnosticContext(id, std::move(args)...)));
  }

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
