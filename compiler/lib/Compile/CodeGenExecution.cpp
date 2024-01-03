#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/IRGenRequest.h"

using namespace stone;

GenerateIRExecution::GenerateIRExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status GenerateIRExecution::ExecuteAction() {

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
          compiler.GetASTContext(), compiler.GetMemoryContext(), psps,
          parallelOutputFilenames));

      compiler.AddIRGenResult(result);

      if (HasConsumer()) {
        GetConsumer()->HandleIRGenResult(result);
      }
    }
  } else {
    compiler.ForEachPrimarySourceFile([&](SourceFile &sourceFile) {
      const PrimaryFileSpecificPaths psps =
          compiler.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
              sourceFile);

      auto result = stone::GenIR(IRGenRequest::ForFile(
          compiler.GetInvocation().GetCodeGenOptions(), &sourceFile,
          psps.outputFilename, compiler.GetASTContext(),
          compiler.GetMemoryContext(), psps));

      compiler.AddIRGenResult(result);
      if (HasConsumer()) {
        GetConsumer()->HandleIRGenResult(result);
      }

      return Status();
    });
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedIRGeneration(compiler);
  }

  if (IsSelf()) {
    // Then we emit
    // stone::EmitIR(
  }
  return Status();
}

Status GenerateIRExecution::FinishAction() { return Status(); }

OptimizeIRExecution::OptimizeIRExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler) {}

Status OptimizeIRExecution::ExecuteAction() {

  // stone::OptimizeIR(compiler.GetIRGen()....)

  // std::unique_ptr<IRCodeOptimizer>
  /// irOptimizer = std::make_uqnique<IROptimizer>(GetCodeGenOptions(),
  /// GetASTContext(), ....);

  if (IsMainAction()) {
    // Then we emit
  }

  return Status();
}

EmitBitCodeExecution::EmitBitCodeExecution(Compiler &compiler,
                                           ActionKind currentAction)
    : CompilerExecution(compiler) {}

Status EmitBitCodeExecution::ExecuteAction() {

  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit-bit-code");
  // GeneratedModule
  // compiler.GetIRGenResult();

  return Status();
}

CompilerExecution *EmitBitCodeExecution::GetConsumer() { return this; }

///< EmitObjectExecution

EmitModuleExecution::EmitModuleExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitModuleExecution::ExecuteAction() {

  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit-module-code");

  // compiler.GetIRGenResult();

  return Status();
}

CompilerExecution *EmitModuleExecution::GetConsumer() { return this; }

///< EmitObjectExecution
EmitObjectExecution::EmitObjectExecution(Compiler &compiler)
    : CompilerExecution(compiler) {
  AddAllowHandleIRGenResult();
}

Status EmitObjectExecution::ExecuteAction() {

  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit object code");

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

// void EmitObjectExecution::CompletedIRGeneration(
//     llvm::ArrayRef<IRGenResult *, 8> results) {}

CompilerExecution *EmitObjectExecution::GetConsumer() { return this; }

///< EmitAssemblyExecution

EmitAssemblyExecution::EmitAssemblyExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status EmitAssemblyExecution::ExecuteAction() {

  CompilerStatsTracer tracer(&compiler.GetStatsReporter(),
                             "emit assembly code");

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

// void EmitAssemblyExecution::CompletedIRGeneration(
//     llvm::ArrayRef<IRGenResult *, 8> results) {}

CompilerExecution *EmitAssemblyExecution::GetConsumer() { return this; }
