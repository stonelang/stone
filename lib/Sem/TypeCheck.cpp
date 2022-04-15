#include "stone/Sem/TypeCheck.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Sem/TypeCheckerListener.h"
#include "stone/Sem/TypeCheckerOptions.h"

using namespace stone;
using namespace stone::syn;

void sem::TypeCheck(syn::SyntaxFile &sf, TypeCheckerOptions &typeCheckerOpts,
                      TypeCheckerListener *pipeline) {
  // TypeChecker checker(su, pipeline);
  // assert(sf.stage == SyntaxFileStage::AtImports);
  // sf.stage = SyntaxFileStage::AtTypeCheck;
}

void sem::TypeCheck(syn::Module &m, TypeCheckerOptions &typeCheckerOpts,
                      TypeCheckerListener *pipeline) {
  // TypeChecker checker
  // assert(sf.stage == SyntaxFileStage::AtImports);
  // sf.stage = SyntaxFileStage::AtTypeCheck;

  // Go through all the files and type-check
}
