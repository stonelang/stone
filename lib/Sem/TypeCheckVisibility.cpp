#include "stone/Sem/TypeChecker.h"

using namespace stone;

/// At a high level, this checks the given declaration's signature does not
/// reference any other declarations that are less visible than the
/// declaration itself. Related checks may also be performed.
void TypeChecker::CheckVisibilityLevel(Decl *D) {}

/// Check the QualType visibility level
void TypeChecker::CheckVisibilityLevel(QualType ty) {}
