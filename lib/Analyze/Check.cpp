#include "stone/Analyze/Check.h"
#include "stone/Analyze/Checker.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void sema::CheckSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                           CheckerPipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void sema::CheckModule(SyntaxFile &sf, Syntax &syntax,
                       CheckerPipeline *pipeline) {
  // Checker checker
}
