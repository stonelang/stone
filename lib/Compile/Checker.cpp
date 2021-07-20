#include "stone/Compile/Checker.h"
#include "stone/Compile/CheckerPipeline.h"
#include "stone/Core/Ret.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::sema;
using namespace stone::syn;

Checker::Checker(SyntaxFile &sf, Syntax &syntax, CheckerPipeline *pipeline)
    : sf(sf), syntax(syntax), pipeline(pipeline) {
  stats.reset(new CheckerStats(*this, syntax.GetTreeContext().GetBasic()));
  syntax.GetTreeContext().GetBasic().GetStatEngine().Register(stats.get());
}

void CheckerStats::Print() {}
