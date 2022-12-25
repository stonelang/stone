#include "stone/Gen/CodeGenContext.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Foreign/ClangContext.h"
#include "stone/Public.h"

using namespace stone;

CodeGenContext::CodeGenContext(llvm::LLVMContext &llvmContext,
                               const CodeGenOptions &genOpts,
                               const ModuleOptions &moduleOpts,
                               const stone::TargetOptions &targetOpts,
                               const LangContext &langContext,
                               ClangContext &clangContext)
    : llvmContext(llvmContext), genOpts(genOpts), moduleOpts(moduleOpts),
      targetOpts(targetOpts), langContext(langContext),
      clangContext(clangContext),
      mod(new llvm::Module(moduleOpts.moduleName, llvmContext)) {
  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
  tm = CreateTargetMachine();
}

CodeGenContext::~CodeGenContext() {}

Safe<llvm::TargetMachine> CodeGenContext::CreateTargetMachine() {

  // clang::TargetInfo &targetInfo =
  // GetClangContext().GetInstance().getTarget();

  // // Setup the empty module
  // GetLLVMModule().setTargetTriple(targetInfo.getTriple().getTriple());
  // GetLLVMModule().setDataLayout(targetInfo.getDataLayoutString());

  // const auto &sdkVersion = targetInfo.getSDKVersion();

  // if (!sdkVersion.empty()) {
  //   cgc.GetLLVMModule().setSDKVersion(sdkVersion);
  // }
  // if (const auto *tvt = targetInfo.getDarwinTargetVariantTriple()) {
  //   cgc.GetModule().setDarwinTargetVariantTriple(tvt->getTriple());
  // }

  // if (auto TVSDKVersion = targetInfo.getDarwinTargetVariantSDKVersion()) {
  //   cgc.GetModule().setDarwinTargetVariantSDKVersion(*TVSDKVersion);
  // }

  return stone::CreateTargetMachine(*this);
}
