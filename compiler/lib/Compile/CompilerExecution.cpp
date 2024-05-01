#include "stone/Compile/CompilerExecution.h"
#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Core.h"
#include "stone/Gen/IRGenRequest.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}

CompilerExecution::~CompilerExecution() {}

void CompilerExecution::CompletedCommandLineParsing(Compiler &compiler) {
  llvm_unreachable("Illegal to handle command line parsing!");
}
void CompilerExecution::CompletedConfiguration(Compiler &compiler) {
  llvm_unreachable("Illegal to handle compiler configuration!");
}
void CompilerExecution::CompletedSyntaxAnalysis(Compiler &result) {
  llvm_unreachable("Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedSyntaxAnalysis(SourceFile &result) {
  llvm_unreachable("Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {
  llvm_unreachable("Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedSemanticAnalysis(Compiler &result) {
  llvm_unreachable("Illegal to handle semantic analysis!");
}
void CompilerExecution::CompletedSemanticAnalysis(SourceFile &result) {
  llvm_unreachable("Illegal to handle semantic analysis!");
}
void CompilerExecution::CompletedSemanticAnalysis(ModuleDecl &result) {
  llvm_unreachable("Illegal to handle semantic analysis!");
}
void CompilerExecution::CompletedIRGeneration(Compiler &result) {
  llvm_unreachable("Illegal to handle IR generation!");
}
void CompilerExecution::CompletedIRGeneration(llvm::Module *result) {
  llvm_unreachable("Illegal to handle IR generation!");
}
void CompilerExecution::CompletedIRGeneration(
    llvm::ArrayRef<llvm::Module *> &results) {
  llvm_unreachable("Illegal to handle IR generation!");
}

void CompilerExecution::Print(ColorStream &stream) const {}

CodeCompletionCallbacks *CompilerExecution::GetCodeCompletionCallbacks() {
  llvm_unreachable("Illegal to handle code completion callbacks!");
}

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
      GetConsumer()->CompletedSyntaxAnalysis(sourceFile);
    }
    return Status();
  });

  if (ShouldNotifyConsumer() && compiler.IsCompileForWholeModule()) {
    GetConsumer()->CompletedSyntaxAnalysis(*compiler.GetMainModule());
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

void ResolveImportsExecution::CompletedSyntaxAnalysis(SourceFile &result) {}

void ResolveImportsExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {}

PrintASTBeforeExecution::PrintASTBeforeExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintASTBeforeExecution::ExecuteAction() {

  // stone::DumpSourceFile(sourceFile, compiler.GetASTContext());
  return Status();
}

void PrintASTBeforeExecution::CompletedSyntaxAnalysis(SourceFile &result) {}
void PrintASTBeforeExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {}

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

void TypeCheckExecution::CompletedSyntaxAnalysis(SourceFile &result) {}

void TypeCheckExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {}

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

void PrintASTAfterExecution::CompletedSyntaxAnalysis(SourceFile &result) {}

void PrintASTAfterExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {}

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

void EmitIRBeforeExecution::CompletedSemanticAnalysis(SourceFile &result) {}
void EmitIRBeforeExecution::CompletedSemanticAnalysis(ModuleDecl &result) {}

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

void EmitIRAfterExecution::CompletedSemanticAnalysis(SourceFile &result) {}

void EmitIRAfterExecution::CompletedSemanticAnalysis(ModuleDecl &result) {}

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

void EmitObjectExecution::CompletedIRGeneration(
    llvm::ArrayRef<llvm::Module *> &results) {}

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

  // stone::GenNative(IRGenOuput,
  //                  GetCodeGenContext().GetLLVMModule().getName());

  // if (compiler.HasObservation()) {
  //   compiler.GetObservation()->CompletedNativeGeneration(compiler);
  // }

  return Status();
}

void EmitAssemblyExecution::CompletedIRGeneration(llvm::Module *result) {}

void EmitAssemblyExecution::CompletedIRGeneration(
    llvm::ArrayRef<llvm::Module *> &results) {}

Status EmitAssemblyExecution::FinishAction() { return Status(); }

/// Handles LLVM
Status stone::CompileAction(Compiler &compiler) {
  assert(CompilerOptions::IsAnyAction(compiler.GetMainAction()));
  return compiler.ExecuteAction(compiler.GetMainAction());
}
/// Handles LLVM
Status stone::CompileLLVM(Compiler &compiler) {

  llvm::Module *llvmModule = nullptr;
  stone::GenNative(compiler.GetInvocation().GetCodeGenOptions(), llvmModule,
                   compiler.GetASTContext(),
                   compiler.GetInvocation()
                       .GetCompilerOptions()
                       .inputsAndOutputs.GetSingleOutputFilename());

  return Status();
}
