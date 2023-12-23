#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/IRGenRequest.h"

using namespace stone;

GenerateIRExecution::GenerateIRExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status GenerateIRExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::EmitIRBefore);

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

      return Status();
    });
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedIRGeneration(compiler);
  }

  if (IsMainAction()) {
    // Then we emit
    // stone::EmitIR(
  }
  return Status();
}

OptimizeIRExecution::OptimizeIRExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status OptimizeIRExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::EmitIRAfter);

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
    : CompilerExecution(compiler, currentAction) {}

Status EmitBitCodeExecution::Execute() {

  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit-bit-code");
  // GeneratedModule
  // compiler.GetIRGenResult();

  return Status();
}

EmitModuleExecution::EmitModuleExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status EmitModuleExecution::Execute() {
  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit-module-code");

  // compiler.GetIRGenResult();

  return Status();
}

EmitNativeExecution::EmitNativeExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status EmitNativeExecution::Execute() {

  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit-native-code");

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

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedNativeGeneration(compiler);
  }

  return Status();
}
