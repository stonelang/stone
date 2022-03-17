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

void Lang::PerformAnalysis(llvm::ArrayRef<SourceUnit *> sources) {

  for (auto source : sources) {
    assert(source);
    PerformAnalysis(*source);
  }
  if (frontend.GetMode().JustParse()) {
    return;
  }
  if (frontend.GetTypeCheckMode() == types::TypeCheckMode::WholeModule) {
    TypeCheckModule(nullptr);
  }
  if (frontend.GetMode().JustTypeCheck()) {
    // Do some things
    return;
  }
}

void Lang::PerformAnalysis(SourceUnit &source) {

  auto syntaxFile = Parse(source.GetSrcID());
  assert(syntaxFile);

  // TODO: May not need this because we are going to add it to the main module.
  // source.SetSyntaxFile(syntaxFile);

  if (frontend.GetMode().IsParse()) {
    return;
  }
  if (frontend.GetMode().IsEmitParse()) {
    // lang.EmitParse(syntaxFile);
    return;
  }
  if (frontend.GetTypeCheckMode() == types::TypeCheckMode::EachFile) {
    TypeCheckSyntaxFile(*syntaxFile);
  }
  if (frontend.GetMode().IsTypeCheck()) {
    return;
  }
  if (frontend.GetMode().IsEmitSyntax()) {
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
  types::TypeCheckSyntaxFile(sf, frontend.GetTypeCheckerOptions());
}
/// Perform type-checking on the entire module
void Lang::TypeCheckModule(syn::Module *mod) {

  assert(mod && "Null 'syn::Module'");
  for (auto mf : mod->GetFiles()) {
    if (auto sf = llvm::dyn_cast<SyntaxFile>(mf))
      TypeCheckSyntaxFile(*sf);
  }
}
