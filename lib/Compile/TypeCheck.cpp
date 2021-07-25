#include "stone/Compile/TypeCheck.h"
#include "stone/Basic/Ret.h"
#include "stone/Compile/TypeChecker.h"
#include "stone/Compile/TypeCheckerOptions.h"
#include "stone/Compile/TypeCheckerPipeline.h"

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
