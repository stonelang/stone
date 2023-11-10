#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/AST/Module.h"
#include "stone/Basic/CompilerDiagnostic.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Basic/TextDiagnosticFormatter.h"
#include "stone/Basic/TextDiagnosticListener.h"
#include "stone/CodeCompletionListener.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerInvocation.h"
#include "stone/Compile/Compiling.h"

#include "stone/Lang.h"
#include "stone/Options/Mode.h"

#include "clang/Basic/TargetInfo.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/Casting.h"

using namespace stone;

// class Compiling final {
//   Compiler& compiler;
//   std::unique_ptr<stone::ASTContext> astContext;
//   std::unique_ptr<ModuleSystem> moduleSystem;
// public:
//   Compiling(Compiler& compiler) : compiler(compiler){}

//  public:

// };

int Lang::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                  void *mainAddr, CompilerListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  auto Finish = [&](Status status = Status::Success()) -> int {
    return status.GetFlag();
  };

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  CompilerInvocation invocation(programName, programPath, listener);
  STONE_DEFER { invocation.Finish(); };

  if (args.empty()) {
    invocation.GetLang().GetDiags().PrintD(SrcLoc(), diag::err_no_input_files);
    return Finish(Status::Error());
  }
  // We setup clang now -- this just loads the instance.
  if (invocation.SetupClang(args, arg0).IsError()) {
    return Finish(Status::Error());
  }

  // Setup the custom formatting to be able to handle asttax diagnostics
  ASTDiagnosticFormatter diagFormatter;
  ASTDiagnosticEmitter diagEmitter(diagFormatter);
  TextDiagnosticListener diagListener(diagEmitter);

  invocation.GetLang().GetDiags().AddListener(diagListener);

  ConfigurationFileBuffers configurationFileBuffers;

  // Parse arguments.
  auto status = invocation.ParseArgs(args);
  if (status.IsError()) {
    return Finish(Status::Error());
  }
  if (invocation.HasError()) {
    return Finish(Status::Error());
  }

  if (invocation.IsAlien()) {
    invocation.GetLang().GetDiags().PrintD(SrcLoc(), diag::err_alien_mode);
    Finish(Status::Error());
  }
  if (invocation.IsPrintHelp()) {
    // TODO: invocation.PrintHelp(invocation.GetOpts());
    return Finish();
  }
  if (invocation.IsPrintVersion()) {
    // invocation.PrintVersion();
    return Finish();
  }
  if (!invocation.CanCompile()) {
    /// invocation.PrintD()
    return Finish();
  }
  invocation.NotifyCompileConfigure();

  if (invocation.CreateSourceBuffers().IsError()) {
    return Finish();
  }

  CompilerInstance compilerInstance(invocation);
  status = compilerInstance.Compile();

  if (status.IsError() || invocation.HasError()) {
    return Finish(Status::Error());
  }
  return Finish();
}

bool CompilerInstance::Compile() {

  assert(CanCompile() && "Unknown mode -- cannot continue with compile!");
  llvm::TimeTraceScope compileTimeScope("Compile");

  auto Finish = [&](Status status = Status::Success()) -> int {
    return status.GetFlag();
  };

  NotifyCompileStarted();

  auto status = Compiling::CompileWithCodeAnalysis(*this);
  if (status.IsError() || status.HasCompletion()) {
    return Finish(status);
  }

  // Some work here before code generation
  status = Compiling::CompilePostCodeAnalysis(*this);

  NotifyCompileFinished();

  return Finish();
}

//// Execute only the analysis part of the compiler
Status Compiling::CompileWithCodeAnalysis(CompilerInstance &compiler) {

  Status status;
  // At this point, everything requires syntax analysis.
  if (compiler.GetMode().IsParse()) {
    if (Compiling::PerformSyntaxAnalysis(compiler).IsError()) {
      return Status::Error();
    }
  }

  // Otherwise, default to performing syntax analysis with import resoltuion.
  if (Compiling::PerformSyntaxAnalysisAndImportResoltuion(compiler).IsError()) {
    status.SetIsError();
    return status;
  }

  if (compiler.GetMode().IsResolveImports()) {
    status.SetHasCompletion();
    return status;
  }

  // Are we trying to dump the AST?
  if (compiler.GetMode().IsDumpAST()) {
    Compiling::PerformDumpAST(compiler);
    status.SetHasCompletion();
    return status;
  }
  // At this point, everything requires type-checking
  if (Compiling::PerformSemanticAnalysis(compiler).IsError()) {
    status.SetIsError();
    return status;
  }

  if (compiler.GetMode().IsTypeCheck()) {
    status.SetHasCompletion();
    return status;
  }
  // Are we trying to print the AST?
  if (compiler.GetMode().IsPrintAST()) {
    Compiling::PerformPrintAST(compiler);
    status.SetHasCompletion();
    return status;
  }

  if (compiler.GetMode().IsDumpTypeInfo()) {
    Compiling::PerformDumpTypeInfo(compiler);
    status.SetHasCompletion();
    return status;
  }
}

Status Compiling::PerformSyntaxAnalysis(CompilerInstance &compiler) {

  for (auto moduleFile :
       compiler.GetModuleSystem().GetMainModule()->GetFiles()) {
    if (auto *astFile = llvm::dyn_cast<stone::ASTFile>(moduleFile)) {
      Lang::ParseASTFile(*astFile, compiler.GetASTContext(),
                         compiler.GetInvocation().GetListener());
    }
  }
  Compiling::NotifySyntaxAnalysisCompleted(compiler);

  return Status();
}

Status Compiling::PerformSyntaxAnalysisAndImportResoltuion(
    CompilerInstance &compiler) {
  if (Compiling::PerformSyntaxAnalysis(compiler).IsError()) {
    return Status::Error();
  }
}
void Compiling::NotifySyntaxAnalysisCompleted(CompilerInstance &compiler) {
  if (compiler.GetInvocation().GetListener()) {
    // compiler.GetInvocation().GetListener()->OnSyntaxAnalysisCompleted(compiler);
  }
}

void Compiling::PerformDumpAST(CompilerInstance &compiler) {}

Status Compiling::PerformSemanticAnalysis(CompilerInstance &compiler) {

  compiler.ForEachASTFile([&](ASTFile &astFile,
                              TypeCheckerOptions &typeCheckerOpts,
                              stone::TypeCheckerListener *listener) {
    Lang::TypeCheckASTFile(astFile, typeCheckerOpts, listener);
  });

  NotifySemanticAnalysisCompleted(compiler);
}

Status Compiling::FinishSemanticAnalysis(CompilerInstance &compiler) {
  return Status();
}

void Compiling::NotifySemanticAnalysisCompleted(CompilerInstance &compiler) {
  if (compiler.GetInvocation().GetListener()) {

    // compiler.GetInvocation().GetListener()->OnSemanticAnalysisCompleted(
    //     compiler);
  }
}
void Compiling::PerformDumpTypeInfo(CompilerInstance &compiler) {}

void Compiling::PerformPrintAST(CompilerInstance &compiler) {}

Status Compiling::CompilePostCodeAnalysis(CompilerInstance &compiler) {

  // Some other things here
  Compiling::CompileWithCodeGeneration(compiler);
}

Status Compiling::CompileWithCodeGeneration(CompilerInstance &compiler) {

  assert(compiler.CanCodeGen() && "Mode does not support code gen");

  llvm::GlobalVariable *hashGlobal;
  auto llvmContext = std::make_unique<llvm::LLVMContext>();
  CodeGenContext codeGenContext(
      compiler.GetInvocation().GetCodeGenOptions(),
      compiler.GetInvocation().GetModuleOptions(),
      compiler.GetInvocation().GetTargetOptions(), *llvmContext,
      compiler.GetASTContext(), compiler.GetInvocation().GetLang(),
      compiler.GetInvocation().GetClang(), hashGlobal);

  //  llvm::StringRef outputFilename = PSPs.OutputFilename;
  //  std::vector<std::string> parallelOutputFilenames =
  //  compiler.GetCompilerOptions().InputsAndOutputs.copyOutputFilenames();

  // At this point, everhing requires IR generations
  if (Compiling::PerformIRGeneration(compiler, codeGenContext).IsError()) {
    return Status::Error();
  }

  // --emit-ir
  if (compiler.GetMode().IsEmitIR()) {
    Compiling::PerformDumpIR(compiler, codeGenContext);
    return Status();
  }

  // --print-ir
  if (compiler.GetMode().IsPrintIR()) {
    Compiling::PerformPrintIR(compiler, codeGenContext);
    return Status();
  }
  /// Do some othere things

  /// --emit-object ....
  // If we are here, we are outputing something native
  if (Compiling::PerformNativeGeneration(compiler, codeGenContext).IsError()) {
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

Status Compiling::PerformIRGeneration(CompilerInstance &compiler,
                                      CodeGenContext &codeGenContext) {

  const auto &invocation = compiler.GetInvocation();
  const CompilerOptions &compilerOpts = invocation.GetCompilerOptions();

  if (compiler.IsWholeModuleCodeGen()) {
    auto *mainModule = compiler.GetModuleSystem().GetMainModule();
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        compiler.GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    Lang::GenIR(codeGenContext, primaryFileSpecificPaths.outputFilename,
                mainModule, primaryFileSpecificPaths);
  } else if (compiler.IsASTFileCodeGen()) {
    for (auto *primaryASTFile : compiler.GetPrimaryASTFiles()) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          compiler.GetPrimaryFileSpecificPathsForASTFile(*primaryASTFile);
      Lang::GenIR(codeGenContext, primaryFileSpecificPaths.outputFilename,
                  primaryASTFile, primaryFileSpecificPaths);
    }
  }
  return Status();
}

Status Compiling::PerformNativeGeneration(CompilerInstance &compiler,
                                          CodeGenContext &codeGenContext) {

  auto result = Lang::GenNative(codeGenContext, llvm::StringRef() /*TODO*/,
                                compiler.GetInvocation().GetListener());

  NotifyNativeGenerationCompleted(compiler);
  return Status::Success();
}

void Compiling::NotifyNativeGenerationCompleted(CompilerInstance &compiler) {}
