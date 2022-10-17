#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

class DeclChecker final : public DeclVisitor<DeclChecker> {
  TypeChecker &checker;

public:
  DeclChecker(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Decl *d) {}
};

void TypeChecker::CheckDecl(Decl *d) { DeclChecker(*this).Visit(d); }
