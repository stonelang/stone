#include "stone/Compile/TypeChecker.h"

using stone::types::TypeChecker;

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
