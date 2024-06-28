#include "stone/Compile/CompilerExecution.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Gen/IRGenRequest.h"
#include "stone/Public.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}
CompilerExecution::~CompilerExecution() {}

void CompilerExecution::CompletedSourceFile(SourceFile& result) {
  assert(false && "Illegal to handle syntax analysis!");

}
void CompilerExecution::CompletedModuleDecl(ModuleDecl& result) {
  assert(false && "Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedIRGeneration(llvm::Module *result) {
  assert(false && "Illegal to handle IR generation!");
}

void CompilerExecution::Print(ColorStream &stream) const {}

CompilerAction CompilerExecution::GetMainAction() {
  return compiler.GetMainAction();
}

void CompilerExecution::VerifyMainActionHasNoConsumer() {
  if (IsMainAction()) {
    assert(!HasConsumer());
  }
}

Compiler &CompilerExecution::GetCompiler() { return compiler; }

CompilerExecution *CompilerExecution::GetConsumer() { return consumer; }

Status CompilerExecution::SetupAction() {

  assert(HasSelfAction());
  if (HasDepAction()) {
    auto execution = compiler.CreateExectution(GetDepAction());
    execution->SetConsumer(this);
    if (compiler.ExecuteAction(*execution).IsError()) {
      return Status::MakeHasCompletionAndIsError();
    }
  }
  return Status();
}

Status CompilerExecution::FinishAction() { return Status(); }

PrintHelpExecution::PrintHelpExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintHelpExecution::ExecuteAction() {

  compiler.GetInvocation().GetCompilerOptions().PrintHelp();
  return Status();
}

PrintHelpHiddenExecution::PrintHelpHiddenExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintHelpHiddenExecution::ExecuteAction() {
  compiler.GetInvocation().GetCompilerOptions().PrintHelp(true);
  return Status();
}

PrintVersionExecution::PrintVersionExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintVersionExecution::ExecuteAction() { return Status(); }

PrintFeatureExecution::PrintFeatureExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintFeatureExecution::ExecuteAction() { return Status(); }

ParseExecution::ParseExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status ParseExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "parse-source-file");
  VerifyMainActionHasNoConsumer();

  CodeCompletionCallbacks *codeCompletionCallbacks = nullptr;
  if (compiler.HasObservation()) {
    codeCompletionCallbacks =
        compiler.GetObservation()->GetCodeCompletionCallbacks();
  }

  compiler.ForEachSourceFileInMainModule([&](SourceFile &sourceFile) {
    stone::ParseSourceFile(sourceFile, GetCompiler().GetASTContext(),
                           codeCompletionCallbacks);
    sourceFile.stage = SourceFileStage::Parsed;
    if (codeCompletionCallbacks) {
      codeCompletionCallbacks->CompletedParseSourceFile(&sourceFile);
    }
    if (ShouldNotifyConsumer() && !compiler.IsCompileForWholeModule()) {
      GetConsumer()->CompletedSourceFile(sourceFile);
    }
    return Status();
  });

  if (ShouldNotifyConsumer() && compiler.IsCompileForWholeModule()) {
    GetConsumer()->CompletedModuleDecl(*compiler.GetMainModule());
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedSyntaxAnalysis(compiler);
  }

  return Status();
}

ResolveImportsExecution::ResolveImportsExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status ResolveImportsExecution::ExecuteAction() {
  // assert(GetExecutionAction() == CompilerAction::ResolveImports);

  FrontendStatsTracer tracer(compiler.GetStats(), "import-resolution");

  VerifyMainActionHasNoConsumer();

  // stone::ResolveSourceFileImports(sourceFile);

  return Status();
}

void ResolveImportsExecution::CompletedSourceFile(SourceFile &result) {}

void ResolveImportsExecution::CompletedModuleDecl(ModuleDecl &result) {}

PrintASTBeforeExecution::PrintASTBeforeExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintASTBeforeExecution::ExecuteAction() {

  // stone::DumpSourceFile(sourceFile, compiler.GetASTContext());
  return Status();
}

void PrintASTBeforeExecution::CompletedSourceFile(SourceFile &result) {}
void PrintASTBeforeExecution::CompletedModuleDecl(ModuleDecl &result) {}

TypeCheckExecution::TypeCheckExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status TypeCheckExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "type-check");

  VerifyMainActionHasNoConsumer();

  compiler.ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
    stone::TypeCheckSourceFile(
        sourceFile, GetCompiler().GetInvocation().GetTypeCheckerOptions());
    sourceFile.stage = SourceFileStage::TypeChecked;

    // if (ShouldNotifyConsumer()) {
    //   GetConsumer()->CompletedSemanticAnalysis(sourceFile);
    // }
    return Status();
  });

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedSemanticAnalysis(compiler);
  }

  return Status();
}

void TypeCheckExecution::CompletedSourceFile(SourceFile &result) {}

void TypeCheckExecution::CompletedModuleDecl(ModuleDecl &result) {}

PrintASTAfterExecution::PrintASTAfterExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintASTAfterExecution::ExecuteAction() {

  // assert(GetExecutionAction() == CompilerAction::PrintASTAfter);

  //  GetCompiler().ForEachSourceFileToTypeCheck([&](SourceFile &sourceFile) {
  //    stone::PrintSourceFile(sourceFile, GetCompiler().GetASTContext());
  //    return Status();
  //  });
  return Status();
}

void PrintASTAfterExecution::CompletedSourceFile(SourceFile &result) {}

void PrintASTAfterExecution::CompletedModuleDecl(ModuleDecl &result) {}

///< EmitIRBeforeExecution
EmitIRBeforeExecution::EmitIRBeforeExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitIRBeforeExecution::ExecuteAction() {

  if (compiler.IsWholeModuleCompile()) {
    if (!compiler.GetInvocation()
             .GetCompilerOptions()
             .inputsAndOutputs.HasPrimaryInputs()) {

      const PrimaryFileSpecificPaths psps =
          compiler.GetInvocation()
              .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

      std::vector<std::string> parallelOutputFilenames =
          compiler.GetInvocation()
              .GetCompilerOptions()
              .inputsAndOutputs.CopyOutputFilenames();

      auto result = stone::GenIR(IRGenRequest::ForModule(
          compiler.GetInvocation().GetCodeGenOptions(),
          compiler.GetMainModule(), psps.outputFilename,
          compiler.GetASTContext(), psps, parallelOutputFilenames));

      if (ShouldNotifyConsumer()) {
        GetConsumer()->CompletedIRGeneration(result->GetLLVMModule());
      }
      if (IsMainAction()) {
        // stone::EmitIR(result->GetLLVMModule());
      }
    }
  } else {
    compiler.ForEachPrimarySourceFile([&](SourceFile &sourceFile) {
      const PrimaryFileSpecificPaths psps =
          compiler.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
              sourceFile);

      auto result = stone::GenIR(IRGenRequest::ForFile(
          compiler.GetInvocation().GetCodeGenOptions(), &sourceFile,
          psps.outputFilename, compiler.GetASTContext(), psps));

      if (ShouldNotifyConsumer()) {
        GetConsumer()->CompletedIRGeneration(result->GetLLVMModule());
      }
      return Status();
    });
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedIRGeneration(compiler);
  }

  return Status();
}

Status EmitIRBeforeExecution::FinishAction() { return Status(); }

void EmitIRBeforeExecution::CompletedSourceFile(SourceFile &result) {}

void EmitIRBeforeExecution::CompletedModuleDecl(ModuleDecl &result) {}

///< EmitIRAfterExecution
EmitIRAfterExecution::EmitIRAfterExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitIRAfterExecution::ExecuteAction() {

  // stone::OptimizeIR(compiler.GetInvocation()....)

  // std::unique_ptr<IRCodeOptimizer>
  /// irOptimizer = std::make_uqnique<IROptimizer>(GetCodeGenOptions(),
  /// GetASTContext(), ....);

  return Status();
}

Status EmitIRAfterExecution::FinishAction() { return Status(); }

void EmitIRAfterExecution::CompletedSourceFile(SourceFile &result) {}

void EmitIRAfterExecution::CompletedModuleDecl(ModuleDecl &result) {}

///< PrintIR

///< EmitIRAfterExecution
PrintIRExecution::PrintIRExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintIRExecution::ExecuteAction() {

  // stone::OptimizeIR(compiler.GetInvocation()....)

  // std::unique_ptr<IRCodeOptimizer>
  /// irOptimizer = std::make_uqnique<IROptimizer>(GetCodeGenOptions(),
  /// GetASTContext(), ....);

  return Status();
}

Status PrintIRExecution::FinishAction() { return Status(); }

EmitBitCodeExecution::EmitBitCodeExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitBitCodeExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "emit-bit-code");
  // GeneratedModule
  // compiler.GetIRGenResult();

  return Status();
}
///< EmitObjectExecution

EmitModuleExecution::EmitModuleExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitModuleExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "emit-module-code");

  // compiler.GetIRGenResult();

  return Status();
}

MergeModulesExecution::MergeModulesExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status MergeModulesExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "merge-modules");

  return Status();
}

///< EmitObjectExecution
EmitObjectExecution::EmitObjectExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitObjectExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "emit object code");

  // if (GenerateIR().IsError()) {
  //   return Status::Error();
  // }

  compiler.TryFreeASTContext();

  // compiler.GetIRGenResult();

  // std::unique_ptr<NativeCodeGen>
  /// nativeCode = std::make_uqnique<NativeCodeGen>(GetCodeGenOptions(),
  /// GetASTContext(), ....);
  // nativeCode->Gen();
  // nativeCode->Optimize();
  // nativeCode->Write();

  // stone::GenNative(IRGenOuput,
  //                  GetCodeGenContext().GetLLVMModule().getName());

  // if (compiler.HasObservation()) {
  //   compiler.GetObservation()->CompletedNativeGeneration(compiler);
  // }

  return Status();
}

void EmitObjectExecution::CompletedIRGeneration(llvm::Module *result) {}

Status EmitObjectExecution::FinishAction() { return Status(); }

///< EmitAssemblyExecution

EmitAssemblyExecution::EmitAssemblyExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitAssemblyExecution::ExecuteAction() {

  FrontendStatsTracer tracer(compiler.GetStats(), "emit assembly code");

  // if (GenerateIR().IsError()) {
  //   return Status::Error();
  // }

  compiler.TryFreeASTContext();

  // compiler.GetIRGenResult();

  // std::unique_ptr<NativeCodeGen>
  /// nativeCode = std::make_uqnique<NativeCodeGen>(GetCodeGenOptions(),
  /// GetASTContext(), ....);
  // nativeCode->Gen();
  // nativeCode->Optimize();
  // nativeCode->Write();

  // stone::EmitNative(IRGenOuput,
  //                  GetCodeGenContext().GetLLVMModule().getName());

  // if (compiler.HasObservation()) {
  //   compiler.GetObservation()->CompletedNativeGeneration(compiler);
  // }

  return Status();
}

void EmitAssemblyExecution::CompletedIRGeneration(llvm::Module *result) {}

Status EmitAssemblyExecution::FinishAction() { return Status(); }

/// Completeds LLVM
Status stone::CompileAction(Compiler &compiler) {
  assert(CompilerOptions::IsAnyAction(compiler.GetMainAction()));
  return compiler.ExecuteAction(compiler.GetMainAction());
}
/// Completeds LLVM
Status stone::CompileLLVM(Compiler &compiler) {

  llvm::Module *llvmModule = nullptr;
  stone::EmitNative(compiler.GetInvocation().GetCodeGenOptions(), llvmModule,
                    compiler.GetASTContext(),
                    compiler.GetInvocation()
                        .GetCompilerOptions()
                        .inputsAndOutputs.GetSingleOutputFilename());

  return Status();
}
