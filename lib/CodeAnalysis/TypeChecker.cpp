#include "stone/CodeAnalysis/TypeChecker.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeAnalysis/TypeCheckerPipeline.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::sema;
using namespace stone::syn;

TypeChecker::TypeChecker(SyntaxFile &sf, Syntax &syntax,
                         TypeCheckerPipeline *pipeline)
    : sf(sf), syntax(syntax), pipeline(pipeline) {
  stats.reset(new TypeCheckerStats(*this, syntax.GetTreeContext().GetBasic()));
  syntax.GetTreeContext().GetBasic().GetStatEngine().Register(stats.get());
}

void TypeCheckerStats::Print() {}
