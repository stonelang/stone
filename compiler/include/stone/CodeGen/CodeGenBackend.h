#ifndef STONE_CODEGEN_BACKEND_H
#define STONE_CODEGEN_BACKEND_H

#include "stone/AST/ASTContext.h"
#include "stone/AST/Module.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/Support/Statistics.h"

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
#include "llvm/Target/TargetOptions.h"
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

namespace stone {

// class CodeGenBackend final {

// public:
//   CodeGenBackend(const CodeGenOptions &codeGenOpts, ASTContext &astContext,
//                  llvm::Module *llvmModule, llvm::StringRef outputFilename,
//                  DiagnosticEngine &diags);

// public:

// };

bool EmitBackendOutput(const CodeGenOptions &Opts, ASTContext &Ctx,
                       llvm::Module *Module, StringRef OutputFilename);

bool EmitBackendOutput(const CodeGenOptions &Opts, DiagnosticEngine &Diags,
                       llvm::sys::Mutex *DiagMutex,
                       llvm::GlobalVariable *HashGlobal, llvm::Module *Module,
                       llvm::TargetMachine *TargetMachine,
                       StringRef OutputFilename, StatsReporter *Stats);

void EmbedBitCode(const CodeGenOptions &Opts, llvm::Module *Module);

void OptimizeLLVM(const CodeGenOptions &Opts, llvm::Module *Module,
                  llvm::TargetMachine *TargetMachine,
                  llvm::raw_pwrite_stream *out);

bool WriteBackendOutputFile();

bool TryOpenBackendOputFile();

void WriteEmptyBackendOutputFiles();

/// Creates a TargetMachine from the IRGen opts and AST Context.
std::unique_ptr<llvm::TargetMachine>
CreateTargetMachine(const CodeGenOptions &codeGenOpts, ASTContext &astContext);

} // namespace stone

#endif
