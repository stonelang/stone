#include "stone/Basic/CodeGenOptions.h"
#include "stone/Core.h"
#include "stone/Gen/IRGenOptimizer.h"
#include "stone/Gen/NativeGen.h"
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
// #include "llvm/Transforms/Coroutines.h"
//  TODO: #include "llvm/Transforms/Coroutines/CoroCleanup.h"
//  TODO: #include "llvm/Transforms/Coroutines/CoroEarly.h"
//  #include "llvm/Transforms/Coroutines/CoroElide.h"
//  #include "llvm/Transforms/Coroutines/CoroSplit.h"
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
// #include "llvm/Transforms/Instrumentation/SanitizerCoverage.h"
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

// static void GenNativeWithParallelization() {}

// static void EmitObject(CodeGenContext &cgc, ASTContext &sc,
//                        llvm::StringRef outputFilename) {}

// static void EmitBC(const CodeGenContext &cgc, ASTContext &sc,
//                    llvm::StringRef outputFilename) {}

// static void EmitAssembly(const CodeGenContext &cgc, ASTContext &sc,
//                          llvm::StringRef outputFilename) {}

// void stone::GenNative(CodeGenContext &cgc, ASTContext &sc,
//                       llvm::StringRef outputFilename,
//                       CodeGenListener *listener) {

//   switch (cgc.GetCodeGenOptions().codeGenOutputKind) {
//   case CodeGenOutputKind::ObjectFile:
//     EmitObject(cgc, sc, outputFilename);
//     break;
//   default:
//     break;
//   }
// }

// static void EmbedBitcode(llvm::Module *mod, CodeGenScope &parentScope) {}

/// CodeGenCompletionCallbacks
bool stone::GenNative(const CodeGenOptions &codeGenOpts,
                      llvm::Module *llvmModule, llvm::StringRef outputFilename,
                      ASTContext &astContext) {

  //  NativeGen nativeGen;
  //  nativeCodeGen.GenCode();
  //  nativeCodeGen.OptimizeCode();
  //  nativeCodeGen.WriteCode();

  // stone::EmbedBitcode(codeGenOpts, llvmModule);

  // CodeGenScope nativeScope(cgc.GetCodeGenOptions());
  //  EmbedBitcode(mod, nativeScope);

  // llvm::Optional<llvm::raw_fd_ostream> outputStream;

  // NativeGen nativeCodeGen;

  // stone::OptimizeIR(codeGenOpts, llvmModule, targetMachine, diags);

  // IRGenOptimizer optimizer(codeGenOpts, llvmModule, llvmTarget, diags);
  // optimizer.optimize();

  return true;
}

void stone::EmbedBitcode(const CodeGenOptions &codeGenOpts,
                         llvm::Module *llvmModule) {}

bool stone::WriteEmptyOutputFiles(
    std::vector<std::string> &parallelOutputFilenames,
    const ASTContext &Context, const CodeGenOptions &opts) {
  return true;
}

// bool stone::GenNative(CodeGenOptions &codegenOpts, llvm::StringRef
// outputFilename,
//                       llvm::sys::Mutex *diagMutex,
//                       llvm::GlobalVariable *hashGlobal,
//                       CodeGenListener *listener) {

// llvm::Optional<llvm::raw_fd_ostream> rawOS;
// if (!outputFilename.empty()) {
//   // Try to open the output file.  Clobbering an existing file is fine.
//   // Open in binary mode if we're doing binary output.
//   llvm::sys::fs::OpenFlags osFlags = llvm::sys::fs::OF_None;
//   std::error_code ec;
//   rawOS.emplace(outputFilename, ec, osFlags);
//   // if (rawOS->has_error() || ec) {
//   //   PrintSync(diags, diagMutex,
//   //                SrcLoc(), diag::error_opening_output,
//   //                outputFilename, ec.message());
//   //   rawOS->clear_error();
//   //   return true;
//   // }
//   if (cgc.GetCodeGenOptions().codeGenOutputKind ==
//       CodeGenOutputKind::LLVMIRPreOptimization) {

//     // Send file to the output stream
//     // cgc.GetLLVMModule()->print(rawOS.value(), nullptr);
//     return false;
//   }
// } else {
//   assert(cgc.GetCodeGenOptions().codeGenOutputKind ==
//              CodeGenOutputKind::LLVMModule &&
//          "No output specified");
// }
//  return true;
//}

/// Returns true is successfull
bool stone::WriteNative(CodeGenOptions &codeGenOpts,
                        llvm::raw_pwrite_stream &out,
                        llvm::sys::Mutex *diagMutex) {

  // switch (cgc.GetCodeGenOptions().codeGenOutputKind) {
  //   case CodeGenOutputKind::ObjectFile:
  //   case CodeGenOutputKind::NativeAssembly:{
  //      cgc.GetLegacyPassManager().add(llvm::createTargetTransformInfoWrapperPass(
  //         cgc.GetTargetMachine().getTargetIRAnalysis()));
  //     bool failed =
  //     cgc.GetTargetMachine().addPassesToEmitFile(cgc.GetTargetMachine(), out,
  //     nullptr,
  //                                                    cgc.GetCodeGenFileType(),
  //                                                    !cgc.GetCodeGenOptions().VerifyWellFormedIR);
  //     // if (failed) {
  //     //   PrintSync(diags, diagMutex, SourceLoc(),
  //     //                diag::error_codegen_init_fail);
  //     //   return true;
  //     break;
  //   }
  //   default:
  //     break;
  //   }

  // if (parentScope) {
  //   parentScope->GetLegacyPassManager().run(cgc->GetLLVMModule());
  // }

  return true;
}

NativeGen::NativeGen(const CodeGenOptions &codeGenOptions,
                     ASTContext &astContext)
    : codeGenOpts(codeGenOptions), astContext(astContext) {}

// void NativeGen::GenCode() {}
//
// void NativeGen::WriteCode() {}

NativeGen::~NativeGen() {}
