#include "stone/Analyze/Check.h"
#include "stone/Analyze/Checker.h"
#include "stone/Analyze/CheckerPipeline.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Syntax.h"

using namespace stone::syn;
using namespace stone::sema;

void stone::CheckSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                            CheckerPipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void stone::CheckModule(SyntaxFile &sf, Syntax &syntax,
                        CheckerPipeline *pipeline) {
  // Checker checker
}
