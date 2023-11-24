#include "stone/Public.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;

void stone::TypeCheckSourceFile(SourceFile &sf,
                                stone::TypeCheckerOptions &typeCheckerOpts,
                                TypeCheckerListener *listener) {

  if (sf.stage == SourceFileStage::TypeChecked) {
    return;
  }
  TypeChecker checker(sf.GetASTContext(), typeCheckerOpts, listener);
  for (auto d : sf.Decls) {
    checker.CheckDecl(d);
  }
  // checker.Check();

  // assert(sf.stage == SourceFileStage::AtImports);
  sf.stage = SourceFileStage::TypeChecked;
}

void stone::TypeCheckWholeModule(ModuleDecl &md,
                                 stone::TypeCheckerOptions &typeCheckerOpts,
                                 TypeCheckerListener *listener) {

  // Go through all the files and type-check -- OK for now
  for (auto *moduleFile : md.GetFiles()) {
    if (auto *nextSourceFile = dyn_cast<SourceFile>(moduleFile)) {
      stone::TypeCheckSourceFile(*nextSourceFile, typeCheckerOpts, listener);
      
    }
  }
}

/// Pretty print the type checked source file.
void stone::PrintSourceFile(SourceFile &sourceFile, ASTContext &astContext) {
}
