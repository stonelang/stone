#include "stone/Basic/CodeGenOptions.h"
#include "stone/Gen/CodeGenScope.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Public.h"
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

// static void
// OptimizeIRUsingLegacyPassManger(llvm::Module *mod, CodeGenOptions
// &codeGenOpts,
//                                 llvm::TargetMachine *targetMachine) {

//   CodeGenScope optimizeIRScope(codeGenOpts);
//   // Goal
//   // optimizeIRScope.GetLegacyPassManager().run(*mod);
// }

// static void
// OptimizeIRUsingModulePassManger(llvm::Module *mod, CodeGenOptions
// &codeGenOpts,
//                                 llvm::TargetMachine *targetMachine) {

//   CodeGenScope optimizeIRScope(codeGenOpts);
//   // Goal
//   // optimizeIRScope.GetModulePassManager().run(
//   //     *mod, optimizeIRScope.GetModulePassManager());
// }

// TODO: Pass CodeGenContext
void stone::OptimizeIR(llvm::Module *mod, const CodeGenOptions &opts,
                       llvm::TargetMachine *target, DiagnosticEngine &diags) {

  // if (codeGenOpts.useLegacyPassManager) {
  //   OptimizeIRUsingLegacyPassManger(mod, codeGenOpts, targetMachine);
  // } else {
  //   OptimizeIRUsingModulePassManger(mod, codeGenOpts, targetMachine);
  // }
}
