#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/IRCodeGenRequest.h"

using namespace stone;

Status GenerateIRExecution::GenForFile() {
  /// IRCodeGenerator
  assert(!IsForModule());
  if (!GetCompiler().GetPrimarySourceFiles().empty()) {
    for (auto *primarySourceFile : GetCompiler().GetPrimarySourceFiles()) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          GetCompiler()
              .GetInvocation()
              .GetPrimaryFileSpecificPathsForSyntaxFile(*primarySourceFile);

      llvm::StringRef outputFilename = primaryFileSpecificPaths.outputFilename;

      auto result = stone::GenIR(IRCodeGenRequest::ForFile(
          GetCompiler().GetInvocation().GetCodeGenOptions(), primarySourceFile,
          outputFilename, GetCompiler().GetASTContext(),
          GetCompiler().GetMemoryContext(), primaryFileSpecificPaths));

      GetCompiler().AddIRCodeGenResult(result);
    }
  }
}

bool GenerateIRExecution::IsForModule() {
  return GetCompiler().GetInvocation().GetCodeGenOptions().isWholeModuleCompile;
}

Status GenerateIRExecution::GenForModule() {

  assert(IsForModule());
  if (!GetCompiler()
           .GetInvocation()
           .GetCompilerOptions()
           .inputsAndOutputs.HasPrimaryInputs()) {

    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        GetCompiler()
            .GetInvocation()
            .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    llvm::StringRef outputFilename = primaryFileSpecificPaths.outputFilename;
    std::vector<std::string> ParallelOutputFilenames =
        GetCompiler()
            .GetInvocation()
            .GetCompilerOptions()
            .inputsAndOutputs.CopyOutputFilenames();

    auto result = stone::GenIR(IRCodeGenRequest::ForModule(
        GetCompiler().GetInvocation().GetCodeGenOptions(),
        GetCompiler().GetMainModule(), outputFilename,
        GetCompiler().GetASTContext(), GetCompiler().GetMemoryContext(),
        primaryFileSpecificPaths, ParallelOutputFilenames));

    GetCompiler().AddIRCodeGenResult(result);
  }

  // Just return for now
  return Status();
}

GenerateIRExecution::GenerateIRExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status GenerateIRExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::EmitIRBefore);

  // std::unique_ptr<IRCodeGen> irCode = std::make_uqnique<IRCodeGen>();
  // irCodeGen->Gen();

  if (IsForModule()) {
    GenForModule();
  } else {
    GenForFile();
  }

  if (IsMainAction()) {
    // Then we emit
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
  // GernatedCode
  // compiler.GetIRCodeGenResult();

  return Status();
}

EmitModuleExecution::EmitModuleExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status EmitModuleExecution::Execute() {

  // compiler.GetIRCodeGenResult();

  return Status();
}

EmitNativeExecution::EmitNativeExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status EmitNativeExecution::Execute() {

  // compiler.GetStatSystem().EnterStatTracer(StatCounterKind::NativeAssembly);

  // if (GenerateIR().IsError()) {
  //   return Status::Error();
  // }

  GetCompiler().TryFreeASTContext();

  // compiler.GetIRCodeGenResult();

  // std::unique_ptr<NativeCodeGen>
  /// nativeCode = std::make_uqnique<NativeCodeGen>(GetCodeGenOptions(),
  /// GetASTContext(), ....);
  // nativeCode->Gen();
  // nativeCode->Optimize();
  // nativeCode->Write();

  // stone::GenNative(IRCodeGenOuput,
  //                  GetCodeGenContext().GetLLVMModule().getName());

  // compiler.GetStatisticEngine().ExitStatisticTracer(StatisTicCounterKind::NativeAssembly);

  return Status();
}
