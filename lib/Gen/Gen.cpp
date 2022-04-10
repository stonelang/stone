#include "stone/Gen/Gen.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Gen/IRGen.h"
#include "stone/Gen/NativeGen.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"
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
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Coroutines.h"
// TODO: #include "llvm/Transforms/Coroutines/CoroCleanup.h"
// TODO: #include "llvm/Transforms/Coroutines/CoroEarly.h"
//#include "llvm/Transforms/Coroutines/CoroElide.h"
//#include "llvm/Transforms/Coroutines/CoroSplit.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/LowerTypeTests.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/IPO/ThinLTOBitcodeWriter.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Transforms/Instrumentation/AddressSanitizer.h"
#include "llvm/Transforms/Instrumentation/BoundsChecking.h"
#include "llvm/Transforms/Instrumentation/GCOVProfiler.h"
#include "llvm/Transforms/Instrumentation/HWAddressSanitizer.h"
#include "llvm/Transforms/Instrumentation/InstrProfiling.h"
#include "llvm/Transforms/Instrumentation/MemorySanitizer.h"
//#include "llvm/Transforms/Instrumentation/SanitizerCoverage.h"
#include "llvm/Transforms/Instrumentation/ThreadSanitizer.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"
#include "llvm/Transforms/Utils/CanonicalizeAliases.h"
#include "llvm/Transforms/Utils/EntryExitInstrumenter.h"
#include "llvm/Transforms/Utils/NameAnonGlobals.h"
#include "llvm/Transforms/Utils/SymbolRewriter.h"
// TODO: #include "llvm/Transforms/Core/UniqueInternalLinkageNames.h"

#include <memory>

using namespace stone;
using namespace stone::syn;

std::unique_ptr<llvm::Module> stone::GenIR(CodeGenContext &cgc,
                                           syn::SyntaxFile &sf,
                                           const Context &ctx,
                                           const OutputFile *output) {
  // IRGen irGen(genOpts);
  // IRModuleEmitter modEmitter(irGen);

  // for (auto *modFile : synMod->GetFiles()) {
  //   if (auto *sf = dyn_cast<SyntaxFile>(modFile)) {
  //     if (sf->stage >= SyntaxFileStage::DidTypeCheck) {
  //       modEmitter.EmitSyntaxFile(*sf);
  //     }
  //   }
  //   // else {
  //   //   modFile->CollectLinkLibraries([&IGM](LinkLibrary LinkLib) {
  //   //     irModule.AddLinkLibrary(linkLib);
  //   //   });
  //   // }
  // }
  // return std::unique_ptr<llvm::Module>(irModule.ReleaseLLVMModule());
  return nullptr;
}

std::unique_ptr<llvm::Module> stone::GenIR(CodeGenContext &cgc, syn::Module &sf,
                                           const Context &ctx,
                                           const OutputFile *output) {
  // IRGen irGen(genOpts);
  // IRModuleEmitter modEmitter(irGen);

  // for (auto *modFile : synMod->GetFiles()) {
  //   if (auto *sf = dyn_cast<SyntaxFile>(modFile)) {
  //     if (sf->stage >= SyntaxFileStage::DidTypeCheck) {
  //       modEmitter.EmitSyntaxFile(*sf);
  //     }
  //   }
  //   // else {
  //   //   modFile->CollectLinkLibraries([&IGM](LinkLibrary LinkLib) {
  //   //     irModule.AddLinkLibrary(linkLib);
  //   //   });
  //   // }
  // }
  // return std::unique_ptr<llvm::Module>(irModule.ReleaseLLVMModule());
  return nullptr;
}
// TODO: Pass LLVMCore
stone::Error stone::GenNative(CodeGenContext &cgc, syn::SyntaxContext &tc,
                              const OutputFile *output) {
  // assert(llvmMod && "Null LLVM Module");
  // ObjGen objGen(llvmMod, genOpts, tc);
  // objGen.Emit();
  return stone::Error();
}

// void stone::GenModule() {}

// void stone::GenAssembly() {}

// void stone::GenBitCode() {}

// void stone::GenLibrary() {}
