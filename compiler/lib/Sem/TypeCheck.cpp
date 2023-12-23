#include "stone/Core.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;

void stone::TypeCheckSourceFile(SourceFile &sourceFile,
                                stone::TypeCheckerOptions &typeCheckerOpts) {

  if (sourceFile.stage == SourceFileStage::TypeChecked) {
    return;
  }
  TypeChecker checker(sourceFile.GetASTContext(), typeCheckerOpts);
  for (auto topLevelDecl : sourceFile.topLevelDecls) {
    checker.CheckDecl(topLevelDecl);
  }
  // checker.Check();

  // assert(sf.stage == SourceFileStage::AtImports);
  sourceFile.stage = SourceFileStage::TypeChecked;
}

void stone::TypeCheckWholeModule(ModuleDecl &md,
                                 stone::TypeCheckerOptions &typeCheckerOpts) {

  // Go through all the files and type-check -- OK for now
  for (auto *moduleFile : md.GetFiles()) {
    if (auto *nextSourceFile = dyn_cast<SourceFile>(moduleFile)) {
      stone::TypeCheckSourceFile(*nextSourceFile, typeCheckerOpts);
    }
  }
}

/// Pretty print the type checked source file.
void stone::PrintSourceFile(SourceFile &sourceFile, ASTContext &astContext) {}
