#include "stone/CodeGen/CodeGenBackend.h"
#include "stone/AST/ASTContext.h"
#include "stone/Basic/CodeGenOptions.h"

static bool ShouldRemoveTargetFeature(llvm::StringRef feature) {
  return feature == "+thumb-mode";
}
std::unique_ptr<llvm::TargetMachine>
stone::CreateTargetMachine(const CodeGenOptions &codeGenOpts,
                           ASTContext &astContext) {
  std::string targetFeatures;
  if (!codeGenOpts.targetFeatures.empty()) {
    llvm::SubtargetFeatures features;
    for (const std::string &feature : codeGenOpts.targetFeatures)
      if (!ShouldRemoveTargetFeature(feature)) {
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

  llvm::CodeGenOptLevel codeGenOptLevel =
      codeGenOpts.ShouldOptimize() ? llvm::CodeGenOptLevel::Default // -Os
                                   : llvm::CodeGenOptLevel ::None;

  // // On Cygwin 64 bit, dlls are loaded above the max address for 32 bits.
  // // This means that the default CodeModel causes generated code to segfault
  // // when run.
  std::optional<llvm::CodeModel::Model> codeModel = std::nullopt;
  if (effectiveTriple.isArch64Bit() &&
      effectiveTriple.isWindowsCygwinEnvironment()) {
    codeModel = llvm::CodeModel::Large;
  }

  llvm::TargetMachine *targetMachine = target->createTargetMachine(
      effectiveTriple.str(), codeGenOpts.targetCPU, targetFeatures,
      codeGenOpts.llvmTargetOpts, codeGenOpts.relocationModel, codeModel,
      codeGenOptLevel);

  if (!targetMachine) {
    assert(false && "failed to create target machine!");
  }
  return std::unique_ptr<llvm::TargetMachine>(targetMachine);
}

void stone::OptimizeLLVM(const CodeGenOptions &Opts, llvm::Module *Module,
                         llvm::TargetMachine *TargetMachine,
                         llvm::raw_pwrite_stream *out) {

  // CodeGenPassManager codeGenPassManager(codeGenOpts, llvmModule);
  // if (codeGenOpts.useLegacyPassManager) {
  //   codeGenPassManager.RunLegacy();
  // } else {
  //   passMgr.Run();
  // }
}

void stone::EmbedBitCode(const CodeGenOptions &Opts, llvm::Module *Module) {}

/// Emit a backend file
bool CodeGenBackend::EmitOutputFile(const CodeGenOptions &Opts, ASTContext &AC,
                                    llvm::Module *Module,
                                    StringRef OutputFilename) {

  // Build TargetMachine.
  auto targetMachine = stone::CreateTargetMachine(Opts, AC);
  if (!targetMachine) {
    return true;
  }
  Module->setDataLayout(AC.GetClangImporter()
                            .GetClangInstance()
                            .getTarget()
                            .getDataLayoutString());
}

bool CodeGenBackend::EmitOutputFile(
    const CodeGenOptions &Opts, DiagnosticEngine &Diags,
    llvm::sys::Mutex *DiagMutex, llvm::GlobalVariable *HashGlobal,
    llvm::Module *Module, llvm::TargetMachine *TargetMachine,
    StringRef OutputFilename, StatsReporter *Stats) {

  // AC.GetClangImporter().createOutputFile();
}

bool CodeGenBackend::WriteOutputFile() {}
