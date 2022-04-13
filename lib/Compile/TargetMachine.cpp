#include "stone/Compile/TargetMachine.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Syntax/SyntaxContext.h"

// static Optional<llvm::CodeModel::Model>
// GetCodeModel(const CodeGenOptions &CodeGenOpts) {
//   unsigned codeModel = llvm::StringSwitch<unsigned>(CodeGenOpts.CodeModel)
//                            .Case("tiny", llvm::CodeModel::Tiny)
//                            .Case("small", llvm::CodeModel::Small)
//                            .Case("kernel", llvm::CodeModel::Kernel)
//                            .Case("medium", llvm::CodeModel::Medium)
//                            .Case("large", llvm::CodeModel::Large)
//                            .Case("default", ~1u)
//                            .Default(~0u);
//   assert(codeModel != ~0u && "invalid code model!");
//   if (codeModel == ~1u){
//     return llvm::None;
//   }
//   return static_cast<llvm::CodeModel::Model>(codeModel);
// }

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
stone::CreateTargetMachine(const CodeGenOptions &codeGenOpts,
                           syn::SyntaxContext &tc) {
  // Create the TargetMachine for generating code.
  // std::string error;
  // std::string Triple = TheModule->getTargetTriple();
  // const llvm::Target *llvmTarget = TargetRegistry::lookupTarget(Triple,
  // error);

  // llvm::Optional<llvm::CodeModel::Model> codeModel =
  // GetCodeModel(codeGenOpts); std::string featuresStr =
  //     llvm::join(targetOpts.features.begin(), targetOpts.features.end(),
  //     ",");
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
