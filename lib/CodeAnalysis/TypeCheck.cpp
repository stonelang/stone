#include "stone/CodeAnalysis/TypeCheck.h"
#include "stone/Basic/Ret.h"
#include "stone/CodeAnalysis/TypeChecker.h"
#include "stone/CodeAnalysis/TypeCheckerPipeline.h"
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
