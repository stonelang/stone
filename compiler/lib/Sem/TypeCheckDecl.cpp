#include "stone/Sem/TypeChecker.h"

#include "stone/Syntax/SyntaxVisitor.h"

using stone::sem::TypeChecker;

class TypeCheckDeclVisitor final : public DeclVisitor<TypeCheckDeclVisitor> {
  TypeChecker &checker;

public:
  TypeCheckDeclVisitor(TypeChecker &checker) : checker(checker) {}

public:
  void Visit(Decl *d) {}
};

void TypeChecker::CheckDecl(Decl *d) { TypeCheckDeclVisitor(*this).Visit(d); }
