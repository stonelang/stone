#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/IRCodeGenRequest.h"

using namespace stone;

Status GenerateIRExecution::GenForFile() {

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
  assert(GetDependencyStatus().IsSuccess());

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
  assert(GetDependencyStatus().IsSuccess());

  // stone::OptimizeIR(compiler.GetIRCodeGen()....)

  if (IsMainAction()) {
    // Then we emit
  }

  return Status();
}

EmitBitCodeExecution::EmitBitCodeExecution(Compiler &compiler,
                                           ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status EmitBitCodeExecution::Execute() {

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

  // if (GenerateIR().IsError()) {
  //   return Status::Error();
  // }

  GetCompiler().TryFreeASTContext();

  // compiler.GetIRCodeGenResult();

  // stone::GenNative(IRCodeGenOuput,
  //                  GetCodeGenContext().GetLLVMModule().getName());

  return Status();
}
