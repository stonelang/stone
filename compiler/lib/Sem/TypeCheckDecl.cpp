#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

class AccessLevelCheckingBase {
public:
};

class AccessLevelChecking : public AccessLevelCheckingBase,
                            public DeclVisitor<AccessLevelChecking> {

  TypeChecker &checker;

public:
  AccessLevelChecking(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Decl *d) {
    // if (d->IsInvalid() || d->IsImplicit()) {
    //   return;
    // }
    DeclVisitor<AccessLevelChecking>::Visit(d);
  }
  // Force all kinds to be handled at a lower level.
  void VisitDecl(Decl *D) {}
  void VisitValueDecl(ValueDecl *D) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}

  // #define UNREACHABLE(KIND, REASON) \
  // void visit##KIND##Decl(KIND##Decl *D) { \
  //   llvm_unreachable(REASON); \
  // }
  // #undef UNREACHABLE
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

    // checker.CheckTypes(d);
  }
  void VisitDecl(Decl *D) {}

public:
  void VisitFunDecl(FunDecl *funDecl) { checker.CheckAccessLevel(funDecl); }

  void VisitImportDecl(ImportDecl *importDecl) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentSyntaxFile();
  DeclChecking(*this, d->GetSyntaxContext(), sf).Visit(d);
}

void TypeChecker::CheckAccessLevel(Decl *d) {

  if (llvm::isa<syn::ValueDecl>(d)) {
    AccessLevelChecking(*this).Visit(d);
  }
}
