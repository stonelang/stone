#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
#include "stone/Compile/Parse.h"
#include "stone/Compile/TypeCheck.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "llvm/IR/Module.h"

using namespace stone;

using stone::LangInstance;
using stone::LangListener;
using stone::ModeKind;
using stone::SyntaxListener;
using stone::syn::SyntaxFile;
using stone::syn::SyntaxFileKind;

void LangInstance::PerformAnalysis(llvm::ArrayRef<SourceUnit *> sources) {
  for (auto source : sources) {
    assert(source);
    PerformAnalysis(*source);
  }
  if (langInvocation.GetMode().JustParse()) {
    return;
  }
  if (langInvocation.GetTypeCheckMode() == types::TypeCheckMode::WholeModule) {
    TypeCheckModule(nullptr /*TODO: get module*/);
  }
  if (langInvocation.GetMode().JustTypeCheck()) {
    // Do some things
    return;
  }
}

void LangInstance::PerformAnalysis(SourceUnit &source) {
  auto syntaxFile = Parse(source.GetSrcID());
  assert(syntaxFile);

  // TODO: May not need this because we are going to add it to the main module.
  // source.SetSyntaxFile(syntaxFile);

  if (langInvocation.GetMode().IsParse()) {
    return;
  }
  if (langInvocation.GetMode().IsEmitParse()) {
    // lang.EmitParse(syntaxFile);
    return;
  }
  if (langInvocation.GetTypeCheckMode() == types::TypeCheckMode::EachFile) {
    TypeCheckSyntaxFile(*syntaxFile);
  }
  if (langInvocation.GetMode().IsTypeCheck()) {
    return;
  }
  if (langInvocation.GetMode().IsEmitSyntax()) {
    // lang.EmitSyntax(*sntaxFile)
  }
}

SyntaxFile *LangInstance::Parse(const unsigned srcID) {
  // TODO: You are not always creating a Library
  auto sf = SyntaxFile::Make(SyntaxFileKind::Library,
                             *GetModuleSystem().GetMainModule(),
                             GetSyntax().GetSyntaxContext(), srcID);

  syn::ParseSyntaxFile(*sf, GetSyntax());
  return sf;
}

void LangInstance::ResolveUse() {}

void LangInstance::TypeCheckSyntaxFile(SyntaxFile &sf) {
  assert(sf.stage == syn::SyntaxFileStage::AtImports);
  types::TypeCheckSyntaxFile(sf, langInvocation.GetTypeCheckerOptions());
}
/// Perform type-checking on the entire module
void LangInstance::TypeCheckModule(syn::Module *mod) {
  assert(mod && "Null 'syn::Module'");
  for (auto mf : mod->GetFiles()) {
    if (auto sf = llvm::dyn_cast<SyntaxFile>(mf))
      TypeCheckSyntaxFile(*sf);
  }
}
