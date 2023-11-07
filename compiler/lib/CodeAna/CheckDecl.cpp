#include "stone/AST/ASTVisitor.h"
#include "stone/Basic/OptionSet.h"
#include "stone/CodeAna/TypeCheckAccess.h"
#include "stone/CodeAna/TypeChecker.h"


using namespace stone;
using namespace stone::codeana;


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
