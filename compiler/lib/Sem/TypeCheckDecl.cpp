#include "stone/Sem/TypeChecker.h"

using stone::sem::TypeChecker;

void TypeChecker::CheckDecl(Decl *d) {
  // TODO: Null check
  switch (d->GetKind()) {
  case DeclKind::Fun:
    CheckFunDecl();
    break;
  default:
    break;
  }
}

void TypeChecker::CheckFunDecl() {}
