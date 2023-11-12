#include "stone/Public.h"
#include "stone/Public.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;
using namespace stone::syn;
using namespace stone::sem;

void stone::TypeCheckSyntaxFile(syn::SyntaxFile &sf,
                                stone::TypeCheckerOptions &typeCheckerOpts,
                                TypeCheckerListener *listener) {

  if (sf.stage == SyntaxFileStage::TypeChecked) {
    return;
  }
  TypeChecker checker(sf.GetSyntaxContext(), typeCheckerOpts, listener);
  for (auto d : sf.Decls) {
    checker.CheckDecl(d);
  }
  // checker.Check();

  // assert(sf.stage == SyntaxFileStage::AtImports);
  sf.stage = SyntaxFileStage::TypeChecked;
}

void stone::TypeCheckWholeModule(syn::ModuleDecl &md,
                                 stone::TypeCheckerOptions &typeCheckerOpts,
                                 TypeCheckerListener *listener) {

  // Go through all the files and type-check -- OK for now
  for (auto *moduleFile : md.GetFiles()) {
    if (auto *nextSyntaxFile = dyn_cast<SyntaxFile>(moduleFile)) {
      stone::TypeCheckSyntaxFile(*nextSyntaxFile, typeCheckerOpts, listener);
    }
  }
}
