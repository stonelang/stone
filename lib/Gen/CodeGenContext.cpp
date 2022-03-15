#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/CodeGenOptions.h"

using namespace stone;

CodeGenContext::CodeGenContext(llvm::LLVMContext &llvmContext,
                               const CodeGenOptions &genOpts)
    : llvmContext(llvmContext), genOpts(genOpts) {

  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(
      llvm::PassBuilder::OptimizationLevel::O2);

  // TODO: llvmModule = std::make_unique<llvm::Module>(genOpts.moduleName,
  // llvmContext);
}
CodeGenContext::~CodeGenContext() {}
