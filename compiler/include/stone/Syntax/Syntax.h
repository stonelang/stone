#ifndef STONE_SYNTAX_SYNTAX_H
#define STONE_SYNTAX_SYNTAX_H

#include "stone/Basic/SrcLoc.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Syntax/Expr.h"
#include "stone/Syntax/Ownership.h"
#include "stone/Syntax/Specifier.h"
#include "stone/Syntax/SyntaxContext.h"
#include "stone/Syntax/SyntaxDiagnosticArgument.h"
#include "stone/Syntax/SyntaxResult.h"
#include "stone/Syntax/Type.h"

namespace stone {
namespace syn {
class Decl;
class DeclContext;
class FunDecl;
class StructDecl;
class Stmt;
class IfStmt;
class SwitchStmt;
class Expr;
class Syntax;
class SyntaxFile;

class Syntax final {
  // Verifier verifier;
  std::unique_ptr<SyntaxContext> sc;

public:
  Syntax(const Syntax &) = delete;
  Syntax(Syntax &&) = delete;
  Syntax &operator=(const Syntax &) = delete;
  Syntax &operator=(Syntax &&) = delete;
  Syntax() = delete;

public:
  Syntax(std::unique_ptr<SyntaxContext> sc);
  ~Syntax();

public:
  Identifier &MakeIdentifier(llvm::StringRef name);

public:
  void VerifyDecl(Decl *d);
  Module *MakeModuleDecl(Identifier &name, bool isMainModule);

  FunDecl *MakeFunDecl(DeclName name, SrcLoc nameLoc, DeclContext *parent);
  void VerifyFunDecl(Decl *d);

  StructDecl *MakeStructDecl(DeclName name, SrcLoc loc, DeclContext *dc);
  StructDecl *MakeInterfaceDecl(DeclName name, SrcLoc loc, DeclContext *dc);

public:
  BraceStmt *MakeBraceStmt(SrcLoc lbloc, llvm::ArrayRef<SyntaxNode> elements,
                           SrcLoc rbloc,
                           llvm::Optional<bool> implicit = llvm::None);

public:
  bool HasError() {
    return GetSyntaxContext().GetContext().GetDiagUnit().HasError();
  }
  Context &GetContext() { return GetSyntaxContext().GetContext(); }
  SyntaxContext &GetSyntaxContext() { return *sc.get(); }

  DeclName MakeDeclName();

public:
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID) {
    return GetSyntaxContext().GetContext().GetDiagUnit().PrintD(
        loc, SyntaxDiagnostic(
                 DiagnosticDetail(diagID, llvm::ArrayRef<diag::Argument>())));
  }
  stone::InFlightDiagnostic PrintD(SrcLoc loc, DiagID diagID,
                                   llvm::ArrayRef<diag::Argument> args) {
    return GetSyntaxContext().GetContext().GetDiagUnit().PrintD(
        loc, SyntaxDiagnostic(DiagnosticDetail(diagID, args)));
  }

  template <typename... ArgTypes>
  stone::InFlightDiagnostic
  PrintD(SrcLoc loc, Diag<ArgTypes...> id,
         typename stone::detail::PassArgument<ArgTypes>::type... args) {
    return GetSyntaxContext().GetContext().GetDiagUnit().PrintD(
        loc, SyntaxDiagnostic(DiagnosticDetail(id, std::move(args)...)));
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
