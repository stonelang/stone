#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/IRCodeGenRequest.h"

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

      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          compiler.GetInvocation()
              .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

      std::vector<std::string> parallelOutputFilenames =
          compiler.GetInvocation()
              .GetCompilerOptions()
              .inputsAndOutputs.CopyOutputFilenames();

      auto result = stone::GenIR(IRCodeGenRequest::ForModule(
          compiler.GetInvocation().GetCodeGenOptions(),
          compiler.GetMainModule(), primaryFileSpecificPaths.outputFilename,
          compiler.GetASTContext(), compiler.GetMemoryContext(),
          primaryFileSpecificPaths, parallelOutputFilenames));

      compiler.AddIRCodeGenResult(result);
    }
  } else {
    compiler.ForEachPrimarySourceFile([&](SourceFile &sourceFile) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          compiler.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
              sourceFile);

      auto result = stone::GenIR(IRCodeGenRequest::ForFile(
          compiler.GetInvocation().GetCodeGenOptions(), &sourceFile,
          primaryFileSpecificPaths.outputFilename, compiler.GetASTContext(),
          compiler.GetMemoryContext(), primaryFileSpecificPaths));

      compiler.AddIRCodeGenResult(result);

      return Status();
    });
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

  // stone::OptimizeIR(compiler.GetIRCodeGen()....)

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
  // compiler.GetIRCodeGenResult();

  return Status();
}

EmitModuleExecution::EmitModuleExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status EmitModuleExecution::Execute() {
  CompilerStatsTracer tracer(&compiler.GetStatsReporter(), "emit-module-code");

  // compiler.GetIRCodeGenResult();

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

  // compiler.GetIRCodeGenResult();

  // std::unique_ptr<NativeCodeGen>
  /// nativeCode = std::make_uqnique<NativeCodeGen>(GetCodeGenOptions(),
  /// GetASTContext(), ....);
  // nativeCode->Gen();
  // nativeCode->Optimize();
  // nativeCode->Write();

  // stone::GenNative(IRCodeGenOuput,
  //                  GetCodeGenContext().GetLLVMModule().getName());

  return Status();
}
