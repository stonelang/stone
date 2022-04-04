#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMContext.h"
#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
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

struct LangInstance::CodeOptimization final {
  friend LangInstance;
  LangInstance &lang;

  CodeOptimization(LangInstance &lang);
  ~CodeOptimization();
};

LangInstance::CodeOptimization::CodeOptimization(LangInstance &lang)
    : lang(lang) {}

LangInstance::CodeOptimization::~CodeOptimization() {}

inline LangInstance::CodeOptimization &LangInstance::GetCodeOptimization() {
  auto pointer = reinterpret_cast<char *>(const_cast<LangInstance *>(this));
  auto offset = llvm::alignAddr((void *)sizeof(*this),
                                llvm::Align(alignof(CodeOptimization)));
  return *reinterpret_cast<LangInstance::CodeOptimization *>(pointer + offset);
}

// Peform code generation
void LangInstance::PerformCodeOptimization(CodeAnalysis &codeAnalysis) {

  if (GetLangInvocation().GetCodeGenOptions().skipOptimization) {
    /// Send the SyntaxFile to the optimizer
    // OptimizeIR(llvmMod);
    return;
  }
}