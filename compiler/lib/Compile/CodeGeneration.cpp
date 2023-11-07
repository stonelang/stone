#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/AST/Module.h"
#include "stone/Basic/Defer.h"
#include "stone/CodeCompletionListener.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Diag/TextDiagnosticListener.h"
#include "stone/Lang.h"
#include "stone/Options/ModeKind.h"
#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;
using namespace stone::ast;

class CodeGeneration final {
public:
  static Status PerformIRGeneration(CompilerInstance &compiler,
                                    CodeGenContext &codeGenContext);

public:
  static void PerformDumpIR(CompilerInstance, CodeGenContext &codeGenContext);
  static void PerformPrintIR(CompilerInstance, CodeGenContext &codeGenContext);

public:
  static Status PerformNativeGeneration(CompilerInstance &compiler,
                                        CodeGenContext &codeGenContext);

  static void NotifyNativeGenerationCompleted(CompilerInstance &compiler);
};

bool CodeGeneration::PerformIRGeneration(CompilerInstance &compiler,
                                         CodeGenContext &codeGenContext) {

  const auto &invocation = GetInvocation();
  const CompilerOptions &compilerOpts = invocation.GetCompilerOptions();

  if (compiler.IsWholeModuleCodeGen()) {
    auto *mainModule = compiler.GetModuleSystem().GetMainModule();
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        compiler.GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    Lang::GenIR(cgc, primaryFileSpecificPaths.outputFilename, mainModule,
                primaryFileSpecificPaths);
  } else if (compiler.IsASTFileCodeGen()) {
    for (auto *primaryASTFile : GetPrimaryASTFiles()) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          compiler.GetPrimaryFileSpecificPathsForASTFile(*primaryASTFile);
      Lang::GenIR(cgc, primaryFileSpecificPaths.outputFilename, primaryASTFile,
                  primaryFileSpecificPaths);
    }
  }
  return Status();
}

bool CodeGeneration::PerformNativeGeneration(CompilerInstance &compiler,
                                             CodeGenContext &codeGenContext) {

  auto result = Lang::GenNative(cgc, GetASTContext(), llvm::StringRef(),
                                GetInvocation().GetListener());
  return Status::Success();

  NotifyNativeGenerationCompleted();
}

bool CodeGeneration::NotifyNativeGenerationCompleted(
    CompilerInstance &compiler) {}

bool Lang::CompileWithCodeGeneration(CompilerInstance &compiler) {

  assert(compiler.CanCodeGen() && "Mode does not support code gen");

  auto llvmContext = std::make_unique<llvm::LLVMContext>();
  CodeGenContext codeGenContext(
      compiler.GetInvocation().GetCodeGenOptions(), *llvmContext,
      compiler.GetInvocation().GetModuleOptions(),
      compiler.GetInvocation().GetTargetOptions(),
      compiler.GetInvocation().GetLang(), compiler.GetInvocation().GetClang());

  // At this point, everhing requires IR generations
  if (CodeGeneration::PerformIRGeneration(compiler, codeGenContext).IsError()) {
    return Status::Error();
  }
  if (compiler.GetMode().IsEmitIR()) {
    CodeGeneration::PerformDumpIR(compiler, codeGenContext);
    return status;
  }

  if (compiler.GetMode().IsPrintIR()) {
    CodeGeneration::PerformPrintIR(compiler, codeGenContext);
    return status;
  }
  /// Do some othere things

  // If we are here, we are outputing something native
  if (CodeGeneration::PerformNativeGeneration(compiler, codeGenContext)
          .IsError()) {
    return Status::Error();
  }

  // auto *Module = IGM.getModule();
  // assert(Module && "Expected llvm:Module for IR generation!");

  // Module->setTargetTriple(IGM.Triple.str());

  // // Set the module's string representation.
  // Module->setDataLayout(IGM.DataLayout.getStringRepresentation());

  // clang::TargetInfo &targetInfo =
  //     GetInvocation().GetClang().GetInstance().getTarget();

  // // Setup the empty module
  // cgc.GetLLVMModule().setTargetTriple(targetInfo.getTriple().getTriple());
  // cgc.GetLLVMModule().setDataLayout(targetInfo.getDataLayoutString());

  // const auto &sdkVersion = targetInfo.getSDKVersion();

  // if (!sdkVersion.empty()) {
  //   cgc.GetLLVMModule().setSDKVersion(sdkVersion);
  // }

  // if (const auto *tvt = targetInfo.getDarwinTargetVariantTriple()) {
  //   cgc.GetModule().setDarwinTargetVariantTriple(tvt->getTriple());
  // }

  // if (auto TVSDKVersion = targetInfo.getDarwinTargetVariantSDKVersion()) {
  //   cgc.GetModule().setDarwinTargetVariantSDKVersion(*TVSDKVersion);
  // }

  // switch (GetInvocation().GetCodeGenOptions().codeGenOutputKind) {
  // // case CodeGenOutputKind::LLVMModule:
  // //   return CompileWithGenIR(cgc, [&](CompilerInstance &compiler,
  // //   CodeGenContext &cgc) {
  // //     return Lang::GenModule(*this, cgc);
  // //   });
  // case CodeGenOutputKind::LLVMIRPreOptimization:
  // case CodeGenOutputKind::LLVMIRPostOptimization:
  //   return CompileWithGenIR(
  //       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //         return DumpIR(*this, cgc);
  //       });
  // // case CodeGenOutputKind::PrintIR:
  // //   return GenIR(
  // //       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  // //         return PrintIR(*this, cgc);
  // //       });
  // default:
  //   return CompileWithGenIR(
  //       cgc, [&](CompilerInstance &compiler, CodeGenContext &cgc) {
  //         return CompileWithGenNative(cgc);
  //       });
  // }
}
