#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
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

class LangInstance::CodeGeneration final {
  friend LangInstance;

public:
  CodeGeneration();
  ~CodeGeneration();
};

void LangInstance::PerformCodeGen() {

  assert(langInvocation.CanCodeGen());

  // We are performing some low leverl code generation
  CodeGenContext cgc(stone::GetLLVMContext(),
                     langInvocation.GetCodeGenOptions());

  // At this point, we much generate IR for all succeeding modes

  // TODO:
  /// Choose what we are going to do
  // auto llvmMod = stone::GenIR(*syntaxFile, cgc);

  // auto mod = stone::GenIR(GetMainModule(), cgc);

  if (langInvocation.GetMode().IsEmitIR()) {
    // EmitIR()
    return;
  }

  if (!langInvocation.GetCodeGenOptions().skipOptimization) {
    /// Send the SyntaxFile to the optimizer
    // OptimizeIR(llvmMod);
  }

  if (langInvocation.GetMode().IsNone() ||
      langInvocation.GetMode().IsEmitObject()) {
    // GenObject(srcID, llvmMod, cgc);
    return;
  }
}
llvm::Module *LangInstance::GenIR(syn::SyntaxFile &sf, CodeGenContext &cc) {
  return nullptr;
}

llvm::Module *LangInstance::GenIR(syn::Module &mod, CodeGenContext &cc) {
  return nullptr;
}
void LangInstance::OptimizeIR(llvm::Module *mod) {
  // stone::OptimizeIR
}

void LangInstance::GenObject(const unsigned srcID, llvm::Module *mod,
                             CodeGenContext &cc) {
  /// TODO: This is the only time we should perform a lookup
  // auto outputFile = langInvocation.ComputeOutputFile(srcID);
  // auto result GenObject(cgc GetSyntaxContext(), outputFile.get());
}
