#include "stone/Compile/TargetMachine.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Compile/LangOptions.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/Support/TargetRegistry.h"

using namespace stone;

static Optional<llvm::CodeModel::Model>
GetCodeModel(const CodeGenOptions &cgc) {
  unsigned codeModel = llvm::StringSwitch<unsigned>(cgc.codeModel)
                           .Case("tiny", llvm::CodeModel::Tiny)
                           .Case("small", llvm::CodeModel::Small)
                           .Case("kernel", llvm::CodeModel::Kernel)
                           .Case("medium", llvm::CodeModel::Medium)
                           .Case("large", llvm::CodeModel::Large)
                           .Case("default", ~1u)
                           .Default(~0u);
  assert(codeModel != ~0u && "invalid code model!");
  if (codeModel == ~1u) {
    return llvm::None;
  }
  return static_cast<llvm::CodeModel::Model>(codeModel);
}

// TODO: cleanup
static llvm::CodeGenOpt::Level
GetOptimizationLevel(const CodeGenOptions &codeGenOpts) {
  switch (codeGenOpts.optimizationLevel) {
  default:
    llvm_unreachable("Invalid optimization level!");
  case OptimizationLevel::None:
    return llvm::CodeGenOpt::None;
  case OptimizationLevel::Less:
    return llvm::CodeGenOpt::Less;
  case OptimizationLevel::Default:
    return llvm::CodeGenOpt::Default;
  case OptimizationLevel::Aggressive:
    return llvm::CodeGenOpt::Aggressive;
  }
}

std::unique_ptr<llvm::TargetMachine>
stone::CreateTargetMachine(const CodeGenOptions &codeGenOpts,
                           const LangOptions &langOpts,
                           syn::SyntaxContext &sc, llvm::Module &llvmModule) {

  // Create the TargetMachine for generating code.
  std::string error;
  std::string triple = llvmModule.getTargetTriple();
  const llvm::Target *llvmTarget =
      llvm::TargetRegistry::lookupTarget(triple, error);

  llvm::Optional<llvm::CodeModel::Model> codeModel = GetCodeModel(codeGenOpts);
  std::string features =
      llvm::join(langOpts.targetOpts.features.begin(), langOpts.targetOpts.features.end(), ",");

  llvm::Reloc::Model relocationModel = codeGenOpts.relocationModel;
  llvm::CodeGenOpt::Level codeGenOptLevel = GetOptimizationLevel(codeGenOpts);

  llvm::TargetOptions llvmTargetOptions;
  // if (!InitTargetOptions(Diags, llvmTargetOptions, codeGenOpts, TargetOpts,
  // LangOpts,
  //                        HSOpts))
  //   return;

  std::unique_ptr<llvm::TargetMachine> tm;
  tm.reset(llvmTarget->createTargetMachine(triple, langOpts.targetOpts.cpu, features,
                                           llvmTargetOptions, relocationModel,
                                           codeModel, codeGenOptLevel));

  return tm;
}
