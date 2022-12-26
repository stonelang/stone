#include "stone/CodeCompletionListener.h"
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
  // sf.stage = SyntaxFileStage::AtTypeCheck;
}

void stone::TypeCheckWholeModule(syn::ModuleDecl &m,
                                 stone::TypeCheckerOptions &typeCheckerOpts,
                                 TypeCheckerListener *listener) {
  // TypeChecker checker
  // assert(sf.stage == SyntaxFileStage::AtImports);
  // sf.stage = SyntaxFileStage::AtTypeCheck;

  // Go through all the files and type-check
}
