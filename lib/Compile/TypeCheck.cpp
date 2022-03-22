#include "stone/Compile/TypeCheck.h"

#include "stone/Compile/TypeChecker.h"
#include "stone/Compile/TypeCheckerListener.h"
#include "stone/Compile/TypeCheckerOptions.h"

using namespace stone;
using namespace stone::syn;

void types::TypeCheckSyntaxFile(syn::SyntaxFile &sf,
                                TypeCheckerOptions &typeCheckerOpts,
                                TypeCheckerListener *pipeline) {
  // TypeChecker checker(su, pipeline);

  assert(sf.stage == SyntaxFileStage::AtImports);

  sf.stage = SyntaxFileStage::AtTypeCheck;
}

void types::TypeCheckModule(syn::SyntaxFile &sf,
                            TypeCheckerOptions &typeCheckerOpts,
                            TypeCheckerListener *pipeline) {
  // TypeChecker checker

  assert(sf.stage == SyntaxFileStage::AtImports);

  sf.stage = SyntaxFileStage::AtTypeCheck;
}
