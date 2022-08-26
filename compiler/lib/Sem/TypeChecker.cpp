#include "stone/Sem/TypeChecker.h"
#include "stone/Sem/TypeCheckerListener.h"
#include "stone/Sem/TypeCheckerOptions.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::sem;
using namespace stone::syn;

TypeChecker::TypeChecker(SyntaxContext &sc, TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerListener *pipeline)
    : sc(sc), typeCheckerOpts(typeCheckerOpts), pipeline(pipeline) {

  stats.reset(new TypeCheckerStats(*this));
  sc.GetContext().GetStatEngine().Register(stats.get());
}

void TypeCheckerStats::Print(ColorfulStream &stream) {}
