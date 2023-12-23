#include "stone/Basic/CodeGenOptions.h"
#include "stone/Core.h"
#include "stone/Gen/IRGenModule.h"
#include "stone/Gen/IRGenOptimizer.h"
#include "stone/Syntax/ASTContext.h"
#include "stone/Syntax/Module.h"

#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/StackSafetyAnalysis.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeWriterPass.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/CodeGen/SchedulerRegistry.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ModuleSummaryIndex.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/LTO/LTOBackend.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace stone;

IRGenOptimizer::IRGenOptimizer(const CodeGenOptions &codeGenOpts,
                               llvm::Module *mod,
                               llvm::TargetMachine *targetMachine,
                               DiagnosticEngine &diags)
    : codeGenOpts(codeGenOpts), mod(mod), targetMachine(targetMachine),
      diags(diags), lfpm(mod) {
  // Register all the ctx analyses with the managers.
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);

  // TODO: get ol from gen options
  mpm = pb.buildPerModuleDefaultPipeline(codeGenOpts.GetOptimizationLevel());
}

void IRGenOptimizer::Optimize() {
  GetPassManager().run(*GetLLVMModule(), GetModuleAnalysisManager());
}

void IRGenOptimizer::OptimizeWithLegacyPassManager() {
  GetLegacyPassManager().run(*GetLLVMModule());
}

// TODO: Pass CodeGenContext
void stone::OptimizeIR(const CodeGenOptions &codeGenOpts,
                       llvm::Module *llvmModule, llvm::TargetMachine *target,
                       DiagnosticEngine &diags) {

  IRGenOptimizer optimizer(codeGenOpts, llvmModule, target, diags);
  if (codeGenOpts.useLegacyPassManager) {
    optimizer.OptimizeWithLegacyPassManager();
  } else {
    optimizer.Optimize();
  }
}
