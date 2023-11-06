#include "stone/AST/TypeCheckerOptions.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/TypeChecker.h"
#include "stone/Lang.h"

using namespace stone;
using namespace stone::ast;
using namespace stone::typecheck;


Status CompilerInstance::CompileWithTypeChecking() {
  return CompileWithTypeChecking([&](CompilerInstance &) { return Status::Success(); });
}

Status CompilerInstance::CompileWithTypeChecking(TypeCheckingCompletedCallback notifiy) {

  auto status = CompileWithParsing();
  if (status.IsError()) {
    return status;
  }
  ForEachASTFile([&](ASTFile &asttaxFile, TypeCheckerOptions &typeCheckerOpts,
                     stone::TypeCheckerListener *listener) {
    Lang::TypeCheckASTFile(asttaxFile, typeCheckerOpts, listener);
  });

  // TODO: FinishTypeCheck();
  if (invocation.GetListener()) {
    invocation.GetListener()->OnSemanticAnalysisCompleted(*this);
  }
  return notifiy(*this);
}


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
