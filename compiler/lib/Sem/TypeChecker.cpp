#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;

TypeChecker::TypeChecker(ASTContext &astContext,
                         TypeCheckerOptions &typeCheckerOpts)
    : astContext(astContext), typeCheckerOpts(typeCheckerOpts) {

  stats.reset(new TypeCheckerStats(*this));

  astContext.GetStats().Register(stats.get());
}

TypeChecker::~TypeChecker() {}

void TypeCheckerStats::Print(ColorStream &stream) {}
