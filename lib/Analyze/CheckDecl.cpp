#include "stone/Analyze/Checker.h"

using namespace stone::sema;

void Checker::CheckDecl(Decl *d) {

  // TODO: Null check
  switch (d->GetType()) {
  case Decl::Type::Fun:
    CheckFunDecl();
    break;
  default:
    break;
  }
}

void Checker::CheckFunDecl() {}
