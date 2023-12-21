#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGenRequest.h"
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
// #include "llvm/Target/TargetMachine.h"
// #include "llvm/Target/TargetOptions.h"
//  #include "llvm/Transforms/Coroutines.h"
//   TODO: #include "llvm/Transforms/Coroutines/CoroCleanup.h"
//   TODO: #include "llvm/Transforms/Coroutines/CoroEarly.h"
//   #include "llvm/Transforms/Coroutines/CoroElide.h"
//   #include "llvm/Transforms/Coroutines/CoroSplit.h"
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

#include "llvm/MC/SubtargetFeature.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include <memory>

using namespace stone;

// static void GenIR(CodeGenOptions& codeGenOptions, ASTContext& astContext,
//                   ModuleDecl *md,
//                   llvm::StringRef moduleName,
//                   const PrimaryFileSpecificPaths paths,
//                   SourceFile *sourceFile, CodeGenListener *listener) {
//
//   IRCodeGen irCodeGen;
//   IRCodeGenModule cgm(irCodeGen, sourceFile, moduleName,
//   paths.outputFilename);
//
//   if (sourceFile) {
//     cgm.EmitSourceFile(*sourceFile);
//   } else if (md) {
//     for (auto *moduleFile : md->GetFiles()) {
//       if (auto *nextSourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
//         if (nextSourceFile->stage >= SourceFileStage::TypeChecked)
//           cgm.EmitSourceFile(*nextSourceFile);
//       } else {
//         // File->CollectLinkLibraries([&IGM](LinkLibrary LinkLib) {
//         //   IGM.addLinkLibrary(LinkLib);
//         // });
//       }
//     }
//   }
// }

// void stone::GenerateIR(CodeGenContext &cgc, llvm::StringRef moduleName,
//                        SourceFile *sf, const PrimaryFileSpecificPaths paths,
//                        CodeGenListener *listener) {
//   assert(sf);
//   GenIR(cgc, moduleName, paths, sf->GetParentModule(), sf, listener);
// }
//
// void stone::GenerateIR(CodeGenContext &cgc, llvm::StringRef moduleName,
//                        ModuleDecl *md, const PrimaryFileSpecificPaths paths,
//                        CodeGenListener *listener) {
//   GenIR(cgc, moduleName, paths, md, nullptr, listener);
// }

// IRCodeGenResult* GenModuleIR(ModuleDecl *md, const CodeGenOptions
// &codeGenOpts,
//                         llvm::StringRef moduleName,
//                         const PrimarySpecificPaths &psps,
//                         ArrayRef<std::string> parallelOutputFilenames,
//                         llvm::GlobalVariable **outModuleHash) {
// }

// IRCodeGenResult* GenSoureFileIR(ModuleFile *mf, const CodeGenOptions
// &codeGenOpts,
//                            llvm::StringRef moduleName,
//                            const PrimarySpecificPaths &psps,
//                            llvm::StringRef privateDiscriminator,
//                            llvm::GlobalVariable **outModuleHash)
// }

IRCodeGenResult *stone::GenIR(IRCodeGenRequest request) {

  IRCodeGen codeGen(request.GetCodeGenOptions(), request.GetASTContext());

  const auto &psps = request.GetPrimaryFileSpecificPaths();

  IRCodeGenModule cgm(codeGen, request.GetPrimarySourceFile(),
                                request.GetModuleName(), psps.outputFilename);
  cgm.Setup();

  return IRCodeGenResult::Create(
      request.GetMemoryContext(), std::move(codeGen.llvmContext),
      std::unique_ptr<llvm::Module>{
          cgm.GetClangCodeGen().ReleaseModule()},
      std::move(codeGen.llvmTargetMachine));
}

/// Disable thumb-mode until debugger support is there.
bool stone::ShouldRemoveTargetFeature(llvm::StringRef feature) {
  return feature == "+thumb-mode";
}

std::unique_ptr<llvm::TargetMachine>
stone::CreateTargetMachine(const CodeGenOptions &codeGenOpts) {

  std::string targetFeatures;
  if (!codeGenOpts.targetFeatures.empty()) {
    llvm::SubtargetFeatures features;
    for (const std::string &feature : codeGenOpts.targetFeatures)
      if (!stone::ShouldRemoveTargetFeature(feature)) {
        features.AddFeature(feature);
      }
    targetFeatures = features.getString();
  }

  const llvm::Triple &effectiveTriple =
      llvm::Triple(codeGenOpts.effectiveClangTriple);
  std::string Error;
  const llvm::Target *target =
      llvm::TargetRegistry::lookupTarget(effectiveTriple.str(), Error);
  if (!target) {
    assert(false && "failed to create target!");
  }

  llvm::CodeGenOpt::Level optLevel = codeGenOpts.ShouldOptimize()
                                         ? llvm::CodeGenOpt::Default // -Os
                                         : llvm::CodeGenOpt::None;

  // // On Cygwin 64 bit, dlls are loaded above the max address for 32 bits.
  // // This means that the default CodeModel causes generated code to segfault
  // // when run.
  llvm::Optional<llvm::CodeModel::Model> codeModel = llvm::None;
  if (effectiveTriple.isArch64Bit() &&
      effectiveTriple.isWindowsCygwinEnvironment()) {
    codeModel = llvm::CodeModel::Large;
  }
  // TODO:
  //  else {
  //    codeModel = GetCodeModel(codeGenOpts);
  //  }
  llvm::TargetMachine *targetMachine = target->createTargetMachine(
      effectiveTriple.str(), codeGenOpts.targetCPU, targetFeatures,
      codeGenOpts.llvmTargetOpts, codeGenOpts.relocationModel, codeModel,
      optLevel);

  if (!targetMachine) {
    assert(false && "failed to create target machine!");
  }
  return std::unique_ptr<llvm::TargetMachine>(targetMachine);
}
