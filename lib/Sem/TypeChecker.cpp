#include "stone/Sem/TypeChecker.h"

using namespace stone;

TypeChecker::TypeChecker(SourceFile &sourceFile) : sourceFile(sourceFile) {}

void TypeChecker::TypeCheckVisibilityLevel(Decl *D) {}

void TypeChecker::TypeCheckVisibilityLevel(QualType ty) {}