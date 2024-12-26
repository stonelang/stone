#include "stone/Sem/TypeChecker.h"

using namespace stone;

TypeChecker::TypeChecker(SourceFile &sourceFile,
                         TypeCheckerOptions &typeCheckerOpts)
    : sourceFile(sourceFile), typeCheckerOpts(typeCheckerOpts) {}

void TypeChecker::CheckVisibilityLevel(Decl *D) {}

void TypeChecker::CheckVisibilityLevel(QualType ty) {}