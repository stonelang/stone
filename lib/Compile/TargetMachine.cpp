#include "stone/Compile/TargetMachine.h"
#include "stone/Basic/CodeGenOptions.h"
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

// static CodeGenOptimizationLevel GetCodeGenOptimizationLevel(const
// CodeGenOptions &codeGenOpts) {
//   switch (codeGenOpts.codeGenOptimizationLevel) {
//   default:
//     llvm_unreachable("Invalid optimization level!");
//   case 0:
//     return CodeGenOptimizationLevel::None;
//   case 1:
//     return CodeGenOptimizationLevel::Less;
//   case 2:
//     return CodeGenOptimizationLevel::Default; // O2/Os/Oz
//   case 3:
//     return CodeGenOptimizationLevel::Aggressive;
//   }
// }

std::unique_ptr<llvm::TargetMachine>
stone::CreateTargetMachine(const CodeGenOptions &codeGenOpts, syn::SyntaxContext &sc,
                           llvm::Module &llvmModule) {

  // CodeGenOpt::Level optimizationLevel = codeGenOpts.CandOptimize()
  //                                           ? CodeGenOpt::Default // -Os
  //                                           : CodeGenOpt::None;

  // Set up TargetOptions and create the target features string.
  // llvm::TargetOptions TargetOpts;
  // std::string cpu;
  // std::string effectiveClangTriple;
  // std::vector<std::string> targetFeaturesArray;
  // std::tie(TargetOpts, CPU, targetFeaturesArray, EffectiveClangTriple)
  //   = getIRTargetOptions(Opts, Ctx);

  // const llvm::Triple &EffectiveTriple = llvm::Triple(EffectiveClangTriple);
  // std::string targetFeatures;
  // if (!targetFeaturesArray.empty()) {
  //   llvm::SubtargetFeatures features;
  //   for (const std::string &feature : targetFeaturesArray)
  //     if (!shouldRemoveTargetFeature(feature)) {
  //       features.AddFeature(feature);
  //     }
  //   targetFeatures = features.getString();
  // }

  // Create the TargetMachine for generating code.
  std::string error;
  std::string triple = llvmModule.getTargetTriple();
  const llvm::Target *llvmTarget =
      llvm::TargetRegistry::lookupTarget(triple, error);

  llvm::Optional<llvm::CodeModel::Model> codeModel = GetCodeModel(codeGenOpts);

  // std::string featuresStr =
  //      llvm::join(targetOpts.features.begin(), targetOpts.features.end(),
  //      ",");

  // llvm::Reloc::Model RM = CodeGenOpts.RelocationModel;
  // CodeGenOpt::Level OptLevel = GetCodeGenOptLevel(CodeGenOpts);

  // llvm::TargetOptions Options;
  // if (!initTargetOptions(Diags, Options, CodeGenOpts, TargetOpts, LangOpts,
  //                        HSOpts))
  //   return;
  // TM.reset(TheTarget->createTargetMachine(Triple, TargetOpts.CPU,
  // FeaturesStr,
  //                                         Options, RM, CM, OptLevel));

  return nullptr;
}
