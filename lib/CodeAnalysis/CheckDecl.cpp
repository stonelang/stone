#include "stone/CodeAnalysis/Checker.h"

using namespace stone::sema;

void Checker::CheckDecl(Decl *d) {

  // TODO: Null check
  switch (d->GetKind()) {
  case DeclKind::Fun:
    CheckFunDecl();
    break;
  default:
    break;
  }
}

void Checker::CheckFunDecl() {}
