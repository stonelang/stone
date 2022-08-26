#include "stone/Sem/TypeChecker.h"

using stone::sem::TypeChecker;

void TypeChecker::CheckDecl(Decl *d) {
  // TODO:
  switch (d->GetKind()) {
  case DeclKind::Fun:
    CheckFunDecl(d);
    break;
  default:
    break;
  }
}

void TypeChecker::CheckFunDecl(Decl *d) {
  // TODO: Valid cast
}
