#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/PrimaryFileSpecificPaths.h"
#include "stone/Core.h"
#include "stone/Gen/IRGenInstance.h"
#include "stone/Gen/IRGenRequest.h"

#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/StringSwitch.h"
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
#include "llvm/TargetParser/Triple.h"
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

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include <memory>

using namespace stone;

IRGenResult *stone::GenIR(IRGenRequest request) {

  IRGenInstance instance(request.GetCodeGenOptions(), request.GetASTContext());

  const auto &psps = request.GetPrimaryFileSpecificPaths();
  IRGenModule gm(instance, request.GetPrimarySourceFile(),
                 request.GetModuleName(), psps.outputFilename);
  gm.Setup();

  for (auto *moduleFile : request.GetFiles()) {
    if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
      gm.EmitSourceFile(*sourceFile);
      // if (auto *synthSFU = sourceFile->GetSynthesizedFile()) {
      //   codeGenModule.EmitSynthesizedFileUnit(*synthSFU);
      // }
    } else {
      // sourceFile->CollectLinkLibraries([&codeGenModule](LinkLibrary linkLib){
      //   codeGenModule.AddLinkLibrary(linkLib);
      // });
    }
  }

  return IRGenResult::Create(
      request.GetASTContext(), std::move(instance.llvmContext),
      std::unique_ptr<llvm::Module>{gm.GetClangCodeGen().ReleaseModule()},
      std::move(instance.llvmTargetMachine));
}

// std::unique_ptr<llvm::Module>
// stone::GenIR(const CodeGenOptions &codeGenOpts, ModuleDecl *moduleDecl,
//              const llvm::StringRef moduleName, ASTContext &astContext,
//              const PrimaryFileSpecificPaths psps,
//              llvm::ArrayRef<std::string> parallelOutputFilenames,
//              llvm::GlobalVariable *outModuleHash) {

//   //return
//   std::unique_ptr<llvm::Module>{cgm.GetClangCodeGen().ReleaseModule()};
//   return nullptr;
// }

// std::unique_ptr<llvm::Module>
// stone::GenIR(const CodeGenOptions &codeGenOpts, ModuleFile *moduleFile,
//              const llvm::StringRef moduleName, ASTContext &astContext,
//              const PrimaryFileSpecificPaths psps,
//              llvm::GlobalVariable *outModuleHash) {

//   //return
//   std::unique_ptr<llvm::Module>{cgm.GetClangCodeGen().ReleaseModule()};
//   return nullptr;
// }

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

  auto optLevel = codeGenOpts.ShouldOptimize()
                      ? llvm::CodeGenOptLevel::Default // -Os
                      : llvm::CodeGenOptLevel ::None;

  // // On Cygwin 64 bit, dlls are loaded above the max address for 32 bits.
  // // This means that the default CodeModel causes generated code to segfault
  // // when run.
  std::optional<llvm::CodeModel::Model> codeModel = std::nullopt;
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
