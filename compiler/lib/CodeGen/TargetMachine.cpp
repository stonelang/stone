#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/TargetOptions.h"
#include "stone/Foreign/ClangContext.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Public.h"
#include "stone/Syntax/SyntaxContext.h"

#include "llvm/MC/SubtargetFeature.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace stone;

// TODO: This is something you can get from clang

static Error InitLLVMTargetOptions(CodeGenContext &cgc,
                                   llvm::TargetOptions &llvmTargetOpts) {

  switch (cgc.GetLangContext().GetLangOptions().threadModelKind) {
  case LangOptions::ThreadModelKind::POSIX:
    llvmTargetOpts.ThreadModel = llvm::ThreadModel::POSIX;
    break;
  case LangOptions::ThreadModelKind::Single:
    llvmTargetOpts.ThreadModel = llvm::ThreadModel::Single;
    break;
  }
  return Error();
}

static Optional<llvm::CodeModel::Model>
GetCodeModel(const CodeGenOptions &codeGenOpts) {

  unsigned codeModel = llvm::StringSwitch<unsigned>(codeGenOpts.codeModel)
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

// std::unique_ptr<llvm::TargetMachine>
// stone::CreateTargetMachine(CodeGenContext &cgc) {

//   std::string error;
//   std::string triple = cgc.GetLLVMModule().getTargetTriple();
//   const llvm::Target *llvmTarget =
//       llvm::TargetRegistry::lookupTarget(triple, error);
//   if (llvmTarget) {

//     llvm::Optional<llvm::CodeModel::Model> codeModel =
//         GetCodeModel(cgc.GetCodeGenOptions());

//     std::string features =
//         llvm::join(cgc.GetTargetOptions().features.begin(),
//                    cgc.GetTargetOptions().features.end(), ",");

//     llvm::Reloc::Model relocationModel =
//         cgc.GetCodeGenOptions().relocationModel;

//     llvm::CodeGenOpt::Level codeGenOptLevel =
//         GetOptimizationLevel(cgc.GetCodeGenOptions());

//     llvm::TargetOptions llvmTargetOpts;
//     auto error = InitLLVMTargetOptions(cgc, llvmTargetOpts);
//     if (error.Has()) {
//       return nullptr;
//     }

//     auto targetMachine = llvmTarget->createTargetMachine(
//         triple, cgc.GetTargetOptions().cpu, features, llvmTargetOpts,
//         relocationModel, codeModel, codeGenOptLevel);

//     return std::unique_ptr<llvm::TargetMachine>(targetMachine);
//   }
//   return nullptr;
// }

std::unique_ptr<llvm::TargetMachine>
stone::CreateTargetMachine(const CodeGenOptions &codeGenOpts) {

  std::string targetFeatures;
  if (!codeGenOpts.targetFeatures.empty()) {
    llvm::SubtargetFeatures features;
    for (const std::string &feature : codeGenOpts.targetFeatures)
      if (!stone::ShouldRemoveTargetFeature(feature)) {
        features.AddFeature(feature);
      }
    targetFeatures = features.getString();
  }

  std::string error;
  const llvm::Triple &effectiveTriple =
      llvm::Triple(codeGenOpts.effectiveClangTriple);

  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(effectiveTriple.str(), error);
  if (!target) {
    assert(false && "Could not create target");
    // Ctx.Diags.diagnose(SourceLoc(), diag::no_llvm_target,
    // effectiveTriple.str(),
    //                    Error);
    // return nullptr;
  }

  llvm::CodeGenOpt::Level optLevel = codeGenOpts.ShouldOptimize()
                                         ? llvm::CodeGenOpt::Default // -Os
                                         : llvm::CodeGenOpt::None;

  // // On Cygwin 64 bit, dlls are loaded above the max address for 32 bits.
  // // This means that the default CodeModel causes generated code to segfault
  // // when run.
  llvm::Optional<llvm::CodeModel::Model> codeModel = llvm::None;
  if (effectiveTriple.isArch64Bit() &&
      effectiveTriple.isWindowsCygwinEnvironment()) {
    codeModel = llvm::CodeModel::Large;
  }
  // TODO:
  //  else {
  //    codeModel = GetCodeModel(codeGenOpts);
  //  }

  // // Create a target machine.
  llvm::TargetMachine *targetMachine = target->createTargetMachine(
      effectiveTriple.str(), codeGenOpts.targetCPU, targetFeatures,
      codeGenOpts.llvmTargetOpts, codeGenOpts.relocationModel, codeModel,
      optLevel);
  if (!targetMachine) {
    // Ctx.Diags.diagnose(SourceLoc(), diag::no_llvm_target,
    //                    EffectiveTriple.str(), "no LLVM target machine");
    assert(false && "Could not create target machine");
    // return nullptr;
  }
  return std::unique_ptr<llvm::TargetMachine>(targetMachine);
}
