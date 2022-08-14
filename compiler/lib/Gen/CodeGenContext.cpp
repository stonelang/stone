#include "stone/Gen/CodeGenContext.h"
#include "stone/Context.h"

using namespace stone;

// static std::unique_ptr<llvm::TargetMachine> CreateTargetMachine() {}

CodeGenContext::CodeGenContext(llvm::LLVMContext &llvmContext,
                               const CodeGenOptions &genOpts,
                               const Context &langContext)
    : llvmContext(llvmContext), genOpts(genOpts), langContext(langContext) {
  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(
      llvm::PassBuilder::OptimizationLevel::O2);

  // targetMachine = CreateTargetMachine();
}
CodeGenContext::~CodeGenContext() {}
