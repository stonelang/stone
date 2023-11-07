#include "stone/AST/ASTVisitor.h"
#include "stone/Basic/OptionSet.h"
#include "stone/Compile/TypeCheckAccess.h"
#include "stone/Compile/TypeChecker.h"

using stone::typecheck::TypeChecker;

class DeclChecking final : public DeclVisitor<DeclChecking> {
  TypeChecker &checker;
  ASTContext &sc;
  ASTFile *sf;

public:
  DeclChecking(TypeChecker &checker, ASTContext &sc, ASTFile *sf)
      : checker(checker), sc(sc), sf(sf) {}

public:
  void Visit(Decl *d) {

    DeclVisitor<DeclChecking>::Visit(d);
    checker.CheckTypes(d);
  }

public:
  void VisitFunDecl(FunDecl *funDecl) { checker.CheckAccessLevel(funDecl); }

  void VisitImportDecl(ImportDecl *importDecl) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentASTFile();
  DeclChecking(*this, d->GetASTContext(), sf).Visit(d);
}
