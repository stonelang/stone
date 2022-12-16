#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

class AccessLevelCheckingBase {

public:
};

class AccessLevelChecking : public AccessLevelCheckingBase,
                            public DeclVisitor<AccessLevelChecking> {
public:
};

class DeclChecking final : public DeclVisitor<DeclChecking> {
  TypeChecker &checker;
  SyntaxContext &sc;
  SyntaxFile *sf;

public:
  DeclChecking(TypeChecker &checker, SyntaxContext &sc, SyntaxFile *sf)
      : checker(checker), sc(sc), sf(sf) {}

public:
  void Visit(Decl *d) {
    //
    DeclVisitor<DeclChecking>::Visit(d);
  }

  // TODO: Think about
  void VisitDecl(Decl *d) { llvm_unreachable("Not yet implemented"); }

public:
  void VisitFunDecl(FunDecl *funDecl) { checker.CheckAccessLevel(funDecl); }
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentSyntaxFile();
  DeclChecking(*this, d->GetSyntaxContext(), sf).Visit(d);
}

void TypeChecker::CheckAccessLevel(Decl *d) {

  if (llvm::isa<syn::ValueDecl>(d)) {
    // AccessControlChecker().visit(D);
    // UsableFromInlineChecker().visit(D);
  }

  // if (llvm::isa<syn::ValueDecl>(D) || isa<PatternBindingDecl>(D)) {
  //   bool allowInlineable =
  //       D->getDeclContext()->isInSpecializeExtensionContext();
  //   AccessControlChecker(allowInlineable).visit(D);
  //   UsableFromInlineChecker().visit(D);
  // } else if (auto *ED = dyn_cast<ExtensionDecl>(D)) {
  //   checkExtensionGenericParamAccess(ED);
  // }
}
