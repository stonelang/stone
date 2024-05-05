#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Gen/IRGenOptimizer.h"
#include "stone/Gen/IRGenPassManager.h"
#include "stone/Public.h"

#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Bitcode/BitcodeWriterPass.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRPrinter/IRPrintingPasses.h"
#include "llvm/Linker/Linker.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Mutex.h"
#include "llvm/Support/Path.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/SubtargetFeature.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/AlwaysInliner.h"
#include "llvm/Transforms/IPO/ThinLTOBitcodeWriter.h"
#include "llvm/Transforms/Instrumentation.h"
#include "llvm/Transforms/Instrumentation/AddressSanitizer.h"
#include "llvm/Transforms/Instrumentation/InstrProfiling.h"
#include "llvm/Transforms/Instrumentation/SanitizerCoverage.h"
#include "llvm/Transforms/Instrumentation/ThreadSanitizer.h"
#include "llvm/Transforms/ObjCARC.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/DCE.h"

#include <memory>

using namespace stone;

bool stone::EmitNativeWithParallelization() { return true; }

class NativeEmitter final {

  const CodeGenOptions &codeGenOpts;
  llvm::Module *llvmModule;
  ASTContext &astContext;
  llvm::StringRef outputFilename;
  // std::unique_ptr<llvm::vfs::FileSystem> vfs;

  IRGenPassManager passMgr;

public:
  NativeEmitter(const CodeGenOptions &codeGenOpts, llvm::Module *llvmModule,
                ASTContext &astContext, llvm::StringRef outputFilename)
      : codeGenOpts(codeGenOpts), llvmModule(llvmModule),
        astContext(astContext), outputFilename(outputFilename),
        passMgr(codeGenOpts, llvmModule) {}

public:
  bool EmitOutput();
};

/// CodeGenCompletionCallbacks
bool stone::EmitNative(const CodeGenOptions &codeGenOpts,
                       llvm::Module *llvmModule, ASTContext &astContext,
                       llvm::StringRef outputFilename) {

  NativeEmitter nativeEmitter(codeGenOpts, llvmModule, astContext,
                              outputFilename);

  /// Now, emit the output
  nativeEmitter.EmitOutput();

  // stone::EmbedBitCode(codeGenOpts, llvmModule);
  // stone::OptimizeIR(codeGenOpts, llvmModule, targetMachine, diags);

  // std::optional<llvm::vfs::OutputFile> outputFile;
  // std::unique_ptr<llvm::ToolOutputFile> outputFile

  //   std::unique_ptr<llvm::ToolOutputFile> openOutputFile(StringRef Path) {
  //   std::error_code EC;
  //   auto F = std::make_unique<llvm::ToolOutputFile>(Path, EC,
  //                                                    llvm::sys::fs::OF_None);
  //   if (EC) {
  //     Diags.Report(diag::err_fe_unable_to_open_output) << Path <<
  //     EC.message(); F.reset();
  //   }
  //   return F;
  // }

  return true;
}

// bool stone::GenNative(CodeGenOptions &codegenOpts, llvm::StringRef
// outputFilename,
//                       llvm::sys::Mutex *diagMutex,
//                       llvm::GlobalVariable *hashGlobal,
//                       CodeGenListener *listener) {

// std::optional<llvm::raw_fd_ostream> rawOS;
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

void stone::EmbedBitCode(const CodeGenOptions &codeGenOpts,
                         llvm::Module *llvmModule) {}

bool stone::WriteEmptyOutputFiles(
    std::vector<std::string> &parallelOutputFilenames,
    const ASTContext &Context, const CodeGenOptions &opts) {
  return true;
}

/// Returns true is successfull
// bool WriteNative(CodeGenOptions &codeGenOpts, llvm::Module *llvmModule,
//                         llvm::TargetMachine *targetMachine,
//                         llvm::raw_pwrite_stream &out,
//                         llvm::raw_pwrite_stream *casid,
//                         llvm::sys::Mutex *diagMutex) {

//   IRGenPassManager passMgr(codeGenOpts, llvmModule);

// switch (codeGenOpts.codeGenOutputKind) {
// case CodeGenOutputKind::ObjectFile:
// case CodeGenOutputKind::NativeAssembly: {

//   passMgr.GetLegacyPassManager().add(
//       llvm::createTargetTransformInfoWrapperPass(
//           targetMachine->getTargetIRAnalysis()));

//   bool failed = targetMachine->addPassesToEmitFile(
//       passMgr.GetLegacyPassManager(), out, nullptr,
//       passMgr.GetLegacyPassManager().GetCodeGenFileType(),
//       !passMgr.GetLegacyPassManager().GetCodeGenOptions().VerifyWellFormedIR,
//       nullptr, casid);

//   // if (failed) {
//   //   PrintSync(diags, diagMutex, SourceLoc(),
//   //                diag::error_codegen_init_fail);
//   //   return true;

//   passMgr.RunLegacy();
//   break;
// }
// default:
//   break;
// }

// if (parentScope) {
//   parentScope->GetLegacyPassManager().run(cgc->GetLLVMModule());
// }

//   return true;
// }
