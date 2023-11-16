#include "stone/Gen/CodeGenContext.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Gen/CodeGenScope.h"
#include "stone/Public.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/ClangContext.h"

using namespace stone;

CodeGenContext::CodeGenContext(const CodeGenOptions &genOpts,
                               const ModuleOptions &moduleOpts,
                               const stone::TargetOptions &targetOpts,
                               ASTContext& astContext,
                               ClangContext &clangContext,
                               llvm::GlobalVariable *outModuleHash)
    : genOpts(genOpts), moduleOpts(moduleOpts), targetOpts(targetOpts), astContext(astContext),
      clangContext(clangContext),
      outModuleHash(outModuleHash),
      llvmTargetMachine(stone::CreateTargetMachine(genOpts)) {

  llvmContext = std::make_unique<llvm::LLVMContext>();
  llvmModule =
      std::make_unique<llvm::Module>(moduleOpts.moduleName, *llvmContext);
}

CodeGenContext::~CodeGenContext() {}

// Safe<llvm::TargetMachine> CodeGenContext::CreateTargetMachine() {

//   // clang::TargetInfo &targetInfo =
//   // GetClangContext().GetInstance().getTarget();

//   // // Setup the empty module
//   // GetLLVMModule().setTargetTriple(targetInfo.getTriple().getTriple());
//   // GetLLVMModule().setDataLayout(targetInfo.getDataLayoutString());

//   // const auto &sdkVersion = targetInfo.getSDKVersion();

//   // if (!sdkVersion.empty()) {
//   //   cgc.GetLLVMModule().setSDKVersion(sdkVersion);
//   // }
//   // if (const auto *tvt = targetInfo.getDarwinTargetVariantTriple()) {
//   //   cgc.GetModule().setDarwinTargetVariantTriple(tvt->getTriple());
//   // }

//   // if (auto TVSDKVersion = targetInfo.getDarwinTargetVariantSDKVersion()) {
//   //   cgc.GetModule().setDarwinTargetVariantSDKVersion(*TVSDKVersion);
//   // }
// }

CodeGenScope::CodeGenScope(const CodeGenOptions &codeGenOpts, llvm::Module *mod,
                           llvm::TargetMachine *targetMachine)
    : codeGenOpts(codeGenOpts), mod(mod), targetMachine(targetMachine),
      lfpm(mod) {

  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
}

CodeGenScope::~CodeGenScope() {}
