#include "stone/Gen/CodeGenContext.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/ModuleOptions.h"
#include "stone/Public.h"

using namespace stone;

// static std::unique_ptr<llvm::TargetMachine> CreateTargetMachine() {}

CodeGenContext::CodeGenContext(llvm::LLVMContext &llvmContext,
                               const CodeGenOptions &genOpts,
                               const ModuleOptions &moduleOpts,
                               const LangContext &langContext,
                               ClangContext &clangContext)
    : llvmContext(llvmContext), genOpts(genOpts), moduleOpts(moduleOpts),
      langContext(langContext), clangContext(clangContext),
      mod(new llvm::Module(moduleOpts.moduleName, llvmContext)) {
  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);

  // targetMachine = CreateTargetMachine();
}

CodeGenContext::~CodeGenContext() {}
