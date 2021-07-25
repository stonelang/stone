#include "stone/Compile/TypeChecker.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Compile/TypeCheckerPipeline.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::sema;
using namespace stone::syn;

TypeChecker::TypeChecker(SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
                         TypeCheckerPipeline *pipeline)
    : sf(sf), typeCheckerOpts(typeCheckerOpts), pipeline(pipeline) {

  stats.reset(new TypeCheckerStats(*this, sf.GetTreeContext().GetBasic()));
  sf.GetTreeContext().GetBasic().GetStatEngine().Register(stats.get());
}

void TypeCheckerStats::Print() {}
