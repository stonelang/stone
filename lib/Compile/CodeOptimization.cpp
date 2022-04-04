#include "stone/Compile/LangInstance.h"
#include "stone/Compile/LangListener.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMContext.h"
#include "stone/Session/ModeKind.h"
#include "stone/Syntax/Module.h"
#include "llvm/IR/Module.h"

using namespace stone;

struct LangInstance::CodeOptimization final {
  friend LangInstance;
  LangInstance &lang;

  CodeOptimization(LangInstance &lang);
  ~CodeOptimization();
};

LangInstance::CodeOptimization::CodeOptimization(LangInstance &lang)
    : lang(lang) {}

LangInstance::CodeOptimization::~CodeOptimization() {}

// Peform code generation
void LangInstance::PerformCodeOptimization(CodeAnalysis &codeAnalysis) {}