#include "stone/Basic/OptionSet.h"
#include "stone/Sem/TypeCheckAccess.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/ASTVisitor.h"

using namespace stone;

class DeclChecking final : public DeclVisitor<DeclChecking> {
  TypeChecker &checker;
  ASTContext &sc;
  SourceFile *sf;

public:
  DeclChecking(TypeChecker &checker, ASTContext &sc, SourceFile *sf)
      : checker(checker), sc(sc), sf(sf) {}

public:
  void Visit(Decl *d) {
    DeclVisitor<DeclChecking>::Visit(d);
    checker.CheckTypes(d);
  }
public:
  void VisitFunDecl(FunDecl *funDecl) { 

    checker.CheckAccessLevel(funDecl);
  }

  void VisitImportDecl(ImportDecl *importDecl) {}
  void VisitIfConfigDecl(IfConfigDecl *ifConfigDecl) {}
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentSourceFile();
  DeclChecking(*this, d->GetASTContext(), sf).Visit(d);
}
