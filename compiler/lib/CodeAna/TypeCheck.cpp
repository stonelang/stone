#include "stone/AST/TypeCheckerOptions.h"
#include "stone/CodeCompletionListener.h"
#include "stone/CodeAna/TypeChecker.h"
#include "stone/Lang.h"

using namespace stone;
using namespace stone::ast;
using namespace stone::codeana;

void Lang::TypeCheckASTFile(ast::ASTFile &sf,
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

void Lang::TypeCheckWholeModule(ast::ModuleDecl &md,
                                stone::TypeCheckerOptions &typeCheckerOpts,
                                TypeCheckerListener *listener) {

  // Go through all the files and type-check -- OK for now
  for (auto *moduleFile : md.GetFiles()) {
    if (auto *nextASTFile = dyn_cast<ASTFile>(moduleFile)) {
      Lang::TypeCheckASTFile(*nextASTFile, typeCheckerOpts, listener);
    }
  }
}
