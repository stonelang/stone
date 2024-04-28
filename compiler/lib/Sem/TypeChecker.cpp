#include "stone/Sem/TypeChecker.h"
#include "stone/AST/ASTContext.h"
#include "stone/AST/TypeCheckerOptions.h"

using namespace stone;

TypeChecker::TypeChecker(ASTContext &astContext,
                         TypeCheckerOptions &typeCheckerOpts)
    : astContext(astContext), typeCheckerOpts(typeCheckerOpts) {}

TypeChecker::~TypeChecker() {}
