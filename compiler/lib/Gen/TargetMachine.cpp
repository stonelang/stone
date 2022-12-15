#include "stone/Basic/CodeGenOptions.h"
#include "stone/Public.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace stone;

// TODO: This is something you can get from clang

static stone::Error InitLLVMTargetOptions(DiagnosticEngine &de,
                                          llvm::TargetOptions &llvmTargetOpts,
                                          const CodeGenOptions &codeGenOpts,
                                          const LangOptions &langOpts) {

  // switch (langOpts.GetThreadModel()) {
  // case CompilerOptions::ThreadModelKind::POSIX:
  //   llvmTargetOpts.ThreadModel = llvm::ThreadModel::POSIX;
  //   break;
  // case CompilerOptions::ThreadModelKind::Single:
  //   llvmTargetOpts.ThreadModel = llvm::ThreadModel::Single;
  //   break;
  // }

  return stone::Error();
}

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

std::unique_ptr<llvm::TargetMachine> stone::CreateTargetMachine(
    DiagnosticEngine &de, const CodeGenOptions &codeGenOpts,
    const TargetOptions &targetOpts, const LangOptions &langOpts,
    syn::SyntaxContext &sc) {

  llvm::TargetOptions targetOptions;
  std::unique_ptr<llvm::TargetMachine> targetMachine;

  llvm::Triple Triple(langOpts.Target);

  // Create the TargetMachine for generating code.
  // std::string error;
  // std::string triple = llvmModule.getTargetTriple();
  // const llvm::Target *llvmTarget =
  //     llvm::TargetRegistry::lookupTarget(triple, error);

  // llvm::Optional<llvm::CodeModel::Model> codeModel =
  // GetCodeModel(codeGenOpts); std::string features =
  //     llvm::join(targetOpts.features.begin(), targetOpts.features.end(),
  //     ",");

  // llvm::Reloc::Model relocationModel = codeGenOpts.relocationModel;
  // llvm::CodeGenOpt::Level codeGenOptLevel =
  // GetOptimizationLevel(codeGenOpts);

  // llvm::TargetOptions llvmTargetOpts;
  // if (InitLLVMTargetOptions(de, llvmTargetOpts, codeGenOpts, langOpts).Has())
  // {
  //   return nullptr;
  // }

  // tm.reset(llvmTarget->createTargetMachine(triple, targetOpts.cpu, features,
  //                                          llvmTargetOpts, relocationModel,
  //                                          codeModel, codeGenOptLevel));

  return targetMachine;
}
