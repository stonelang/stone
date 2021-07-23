#include "stone/Basic/Ret.h"
#include "stone/CodeAnalysis/Check.h"
#include "stone/CodeAnalysis/Checker.h"
#include "stone/CodeAnalysis/CheckerPipeline.h"
#include "stone/Syntax/Syntax.h"

using namespace stone;
using namespace stone::syn;

void sema::TypeCheckSyntaxFile(SyntaxFile &sf, Syntax &syntax,
                           TypeCheckerPipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void sema::TypeCheckModule(SyntaxFile &sf, Syntax &syntax,
                       TypeCheckerPipeline *pipeline) {
  // Checker checker
}
