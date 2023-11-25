#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

CodeGenExecution::CodeGenExecution(Compiler &compiler, ActionKind currentAction)
    : CompilerExecution(compiler, currentAction),
      llvmContext(new llvm::LLVMContext()) {}

Status CodeGenExecution::Execute() {

  // CodeGenContext codeGenContext(
  //     compiler.GetInvocation().GetCodeGenOptions(),
  //     compiler.GetInvocation().GetModuleOptions(),
  //     compiler.GetInvocation().GetTargetOptions(), *llvmContext,
  //     compiler.GetASTContext(), compiler.GetInvocation().GetLang(),
  //     compiler.GetInvocation().GetClang(), hashGlobal);

  switch (GetExecutionAction()) {
  default:
    llvm_unreachable("Invalid action for CodeGeneration");
  }
}

Status CodeGenExecution::ExecuteGenerateIR(CodeGenContext &codeGenContext) {

  if (compiler.IsCompileForWholeModule()) {
    auto *mainModule = compiler.GetMainModule();
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        compiler.GetInvocation()
            .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

    // We take the all the files and generate a module
    stone::GenWholeModuleIR(codeGenContext,
                            primaryFileSpecificPaths.outputFilename, mainModule,
                            primaryFileSpecificPaths);
  } else if (compiler.IsCompileForSourceFile()) {
    for (auto *primarySyntaxFile : compiler.GetPrimarySourceFiles()) {
      const PrimaryFileSpecificPaths primaryFileSpecificPaths =
          compiler.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
              *primarySyntaxFile);
      stone::GenSourceFileIR(codeGenContext,
                             primaryFileSpecificPaths.outputFilename,
                             primarySyntaxFile, primaryFileSpecificPaths);
    }
  } else {
    llvm_unreachable("Can only generate IR for a module or a source file!");
  }
  return Status();
}

Status CodeGenExecution::ExecuteGenerateNative(CodeGenContext &codeGenContext) {
  // Before we GenNative, it is possible we may not longer need the AST
  compiler.TryFreeASTContext();

  return Status();
}

EmitIRBeforeExecution::EmitIRBeforeExecution(Compiler &compiler,
                                             ActionKind currentAction)
    : CodeGenExecution(compiler, currentAction) {}

Status EmitIRBeforeExecution::Execute() {

  // ExecuteGenerateIR();
  return Status();
}

EmitIRAfterExecution::EmitIRAfterExecution(Compiler &compiler,
                                           ActionKind currentAction)
    : CodeGenExecution(compiler, currentAction) {}

Status EmitIRAfterExecution::Execute() {
  // ExecuteGenerateIR();

  return Status();
}

EmitNativeExecution::EmitNativeExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CodeGenExecution(compiler, currentAction) {}

Status EmitNativeExecution::Execute() {
  // ExecuteGenerateIR();

  return Status();
}