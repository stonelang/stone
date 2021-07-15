#include "stone/Compile/Check.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/Checker.h"
#include "stone/Compile/CheckerPipeline.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void sema::CheckSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                           sema::CheckerPipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void sema::CheckModule(SyntaxFile &sf, Syntax &syntax,
                       sema::CheckerPipeline *pipeline) {
  // Checker checker
}
