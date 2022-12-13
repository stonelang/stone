#include "stone/Sem/TypeCheck.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Sem/TypeChecker.h"
#include "stone/Syntax/TypeCheckerOptions.h"

using namespace stone;
using namespace stone::syn;

void sem::TypeCheck(syn::SyntaxFile &sf,
                    stone::TypeCheckerOptions &typeCheckerOpts,
                    TypeCheckerListener *listener) {

  TypeChecker checker(sf.GetSyntaxContext(), typeCheckerOpts, listener);
  // for (auto d : sf.Decls) {
  //   checker.CheckDecl(d);
  // }
  // checker.Check();

  // assert(sf.stage == SyntaxFileStage::AtImports);
  // sf.stage = SyntaxFileStage::AtTypeCheck;
}

void sem::TypeCheck(syn::Module &m, stone::TypeCheckerOptions &typeCheckerOpts,
                    TypeCheckerListener *pipeline) {
  // TypeChecker checker
  // assert(sf.stage == SyntaxFileStage::AtImports);
  // sf.stage = SyntaxFileStage::AtTypeCheck;

  // Go through all the files and type-check
}
