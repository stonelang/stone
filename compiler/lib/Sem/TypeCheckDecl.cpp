#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

class DeclChecker final : public DeclVisitor<DeclChecker> {
  TypeChecker &checker;
  SyntaxContext &sc;
  SyntaxFile *sf;

public:
  DeclChecker(TypeChecker &checker, SyntaxContext &sc, SyntaxFile *sf)
      : checker(checker), sc(sc), sf(sf) {}

public:
  void Visit(Decl *d) { DeclVisitor<DeclChecker>::Visit(d); }

  // TODO: Think about
  void VisitDecl(Decl *d) { llvm_unreachable("Not yet implemented"); }

public:
  void VisitFunDecl(FunDecl *funDecl) { checker.CheckAccessLevel(funDecl); }
};

void TypeChecker::CheckDecl(Decl *d) {

  auto *sf = d->GetDeclContext()->GetParentSyntaxFile();
  DeclChecker(*this, d->GetSyntaxContext(), sf).Visit(d);
}

void TypeChecker::CheckAccessLevel(Decl *d) {}
