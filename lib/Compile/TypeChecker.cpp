#include "stone/Compile/TypeChecker.h"
#include "stone/Compile/TypeCheckerListener.h"
#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::types;
using namespace stone::syn;

TypeChecker::TypeChecker(SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerListener *pipeline)
    : sf(sf), typeCheckerOpts(typeCheckerOpts), pipeline(pipeline) {

  stats.reset(new TypeCheckerStats(*this, sf.GetSyntaxContext().GetContext()));
  sf.GetSyntaxContext().GetContext().GetStatEngine().Register(stats.get());
}

void TypeCheckerStats::Print() {}
