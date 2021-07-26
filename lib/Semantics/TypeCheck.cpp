#include "stone/Semantics/TypeCheck.h"
#include "stone/Basic/Ret.h"
#include "stone/Semantics/TypeChecker.h"
#include "stone/Semantics/TypeCheckerOptions.h"
#include "stone/Semantics/TypeCheckerPipeline.h"

using namespace stone;
using namespace stone::syn;

void sema::TypeCheckSyntaxFile(SyntaxFile &sf,
                               TypeCheckerOptions &typeCheckerOpts,
                               TypeCheckerPipeline *pipeline) {
  // Checker checker(su, pipeline);
}

void sema::TypeCheckModule(SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
                           TypeCheckerPipeline *pipeline) {
  // Checker checker
}
