#include "stone/Compile/Lang.h"
#include "stone/Compile/LangListener.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/TypeCheck.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Option/ModeKind.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

using namespace stone;

using stone::Lang;
using stone::LangListener;
using stone::ModeKind;
using stone::SyntaxListener;
using stone::syn::SyntaxFile;
using stone::syn::SyntaxFileKind;

void Lang::PerformCodeAnalysis() {

  for (auto source : lc.sources) {
    PerformCodeAnalysis(source.first);
  }
  if (lc.GetMode().JustParse()) {
    return;
  }
  if (lc.GetTypeCheckMode() == types::TypeCheckMode::WholeModule) {
    TypeCheckModule(nullptr);
  }
  if (lc.GetMode().JustTypeCheck()) {
    // Do some things
    return;
  }
}

void Lang::PerformCodeAnalysis(const unsigned srcID) {

  auto syntaxFile = Parse(srcID);
  if (lc.GetMode().IsParse()) {
    return;
  }
  if (lc.GetMode().IsEmitParse()) {
    // lang.EmitParse(syntaxFile);
    return;
  }
  if (lc.GetTypeCheckMode() == types::TypeCheckMode::EachFile) {
    TypeCheckSyntaxFile(*syntaxFile);
  }
  if (lc.GetMode().IsTypeCheck()) {
    return;
  }
  if (lc.GetMode().IsEmitSyntax()) {
    // lang.EmitSyntax(*sntaxFile)
  }
}

SyntaxFile *Lang::Parse(const unsigned srcID) {

  // Create syntax file
  auto sf = SyntaxFile::Make(SyntaxFileKind::Library,
                             *GetModuleSystem().GetMainModule(),
                             GetSyntaxContext(), srcID);
  syn::ParseSyntaxFile(*sf, GetSyntax());
  return sf;
}

void Lang::TypeCheckSyntaxFile(SyntaxFile &sf) {
  assert(sf.stage == syn::SyntaxFileStage::AtImports);
  types::TypeCheckSyntaxFile(sf, lc.GetTypeCheckerOptions());
}
/// Perform type-checking on the entire module
void Lang::TypeCheckModule(syn::Module *mod) {

  assert(mod && "Null 'syn::Module'");
  for (auto mf : mod->GetFiles()) {
    if (auto sf = llvm::dyn_cast<SyntaxFile>(mf))
      TypeCheckSyntaxFile(*sf);
  }
}
