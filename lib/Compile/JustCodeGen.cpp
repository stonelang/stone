#include "stone/Compile/Lang.h"
#include "stone/Compile/LangListener.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/Gen.h"
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

void Lang::PerformCodeGen() {

  assert(CanCodeGen());

  // We are performing some low leverl code generation
  CodeGenContext cgc(stone::GetLLVMContext(), lc.GetCodeGenOptions());

  // At this point, we much generate IR for all succeeding modes

  // TODO:
  /// Choose what we are going to do
  // auto llvmMod = stone::GenIR(*syntaxFile, cgc);

  // auto mod = stone::GenIR(GetMainModule(), cgc);

  if (lc.GetMode().IsEmitIR()) {
    // EmitIR()
    return;
  }

  if (!lc.GetCodeGenOptions().skipOptimization) {
    /// Send the SyntaxFile to the optimizer
    // OptimizeIR(llvmMod);
  }

  if (lc.GetMode().IsNone() || lc.GetMode().IsEmitObject()) {
    // GenObject(srcID, llvmMod, cgc);
    return;
  }
}
llvm::Module *Lang::GenIR(syn::SyntaxFile &sf, CodeGenContext &cc) {

  return nullptr;
}

llvm::Module *Lang::GenIR(syn::Module &mod, CodeGenContext &cc) {

  return nullptr;
}
void Lang::OptimizeIR(llvm::Module *mod) {

  // stone::OptimizeIR
}

void Lang::GenObject(const unsigned srcID, llvm::Module *mod,
                     CodeGenContext &cc) {

  /// TODO: This is the only time we should perform a lookup
  // auto outputFile = lc.ComputeOutputFile(srcID);
  // auto result GenObject(cgc GetSyntaxContext(), outputFile.get());
}
