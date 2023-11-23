#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

CodeGenExecution::CodeGenExecution(Compiler &compiler, ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status CodeGenExecution::Execute() {

  compiler.GetInvocation().AssertCanEmitCode(GetExecutionAction());

  llvm::GlobalVariable *hashGlobal;
  auto llvmContext = std::make_unique<llvm::LLVMContext>();

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

Status CodeGenExecution::ExecuteGenIR(CodeGenContext &codeGenContext) {

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

Status CodeGenExecution::ExecuteGenNative(CodeGenContext &codeGenContext) {

  // Before we GenNative, it is possible we may not longer need the AST
  compiler.TryFreeASTContext();

  return Status();
}
