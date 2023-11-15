#include "stone/Sem/TypeChecker.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;
using namespace stone::sem;
using namespace stone::syn;

TypeChecker::TypeChecker(ASTContext &sc, TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerListener *pipeline)
    : sc(sc), typeCheckerOpts(typeCheckerOpts), pipeline(pipeline) {

  stats.reset(new TypeCheckerStats(*this));
  sc.GetLangContext().GetStats().Register(stats.get());
}

TypeChecker::~TypeChecker() {}

void TypeCheckerStats::Print(ColorStream &stream) {}
