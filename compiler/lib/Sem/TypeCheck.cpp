#include "stone/Public.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;

void stone::TypeCheckASTFile(ASTFile &sf,
                             stone::TypeCheckerOptions &typeCheckerOpts,
                             TypeCheckerListener *listener) {

  if (sf.stage == ASTFileStage::TypeChecked) {
    return;
  }
  TypeChecker checker(sf.GetASTContext(), typeCheckerOpts, listener);
  for (auto d : sf.Decls) {
    checker.CheckDecl(d);
  }
  // checker.Check();

  // assert(sf.stage == ASTFileStage::AtImports);
  sf.stage = ASTFileStage::TypeChecked;
}

void stone::TypeCheckWholeModule(ModuleDecl &md,
                                 stone::TypeCheckerOptions &typeCheckerOpts,
                                 TypeCheckerListener *listener) {

  // Go through all the files and type-check -- OK for now
  for (auto *moduleFile : md.GetFiles()) {
    if (auto *nextASTFile = dyn_cast<ASTFile>(moduleFile)) {
      stone::TypeCheckASTFile(*nextASTFile, typeCheckerOpts, listener);
    }
  }
}
