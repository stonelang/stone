#include "stone/CodeGen/CodeGenPassManager.h"


using namespace stone;

CodeGenPassManager::CodeGenPassManager(const CodeGenOptions &codeGenOpts,
                                       llvm::Module *llvmModule)

    : codeGenOpts(codeGenOpts), llvmModule(llvmModule), lfpm(llvmModule) {

  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);
  mpm = pb.buildPerModuleDefaultPipeline(codeGenOpts.GetOptimizationLevel());
}

void CodeGenPassManager::RunPasses() {
  GetPassManager().run(*llvmModule, GetModuleAnalysisManager());
}

void CodeGenPassManager::RunLegacyPasses() {
  GetLegacyPassManager().run(*llvmModule);
}
