#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

CodeGenExecution::CodeGenExecution(Compiler &compiler, ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status CodeGenExecution::Execute() {

  switch (GetExecutionAction()) {
  default:
    llvm_unreachable("Invalid action for CodeGeneration");
  }
}

Status CodeGenExecution::ExecuteGenIR(CodeGenContext &codeGenContext) {

  // const auto &invocation = GetInvocation();
  // const CompilerOptions &compilerOpts = invocation.GetCompilerOptions();

  // if (IsWholeModuleCodeGen()) {
  //   auto *mainModule = GetModuleSystem().GetMainModule();
  //   const PrimaryFileSpecificPaths primaryFileSpecificPaths =
  //       GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();

  //   stone::GenModuleIR(cgc, primaryFileSpecificPaths.outputFilename,
  //   mainModule,
  //                      primaryFileSpecificPaths);
  // } else if (IsSyntaxFileCodeGen()) {
  //   for (auto *primarySyntaxFile : GetPrimarySyntaxFiles()) {
  //     const PrimaryFileSpecificPaths primaryFileSpecificPaths =
  //         GetPrimaryFileSpecificPathsForSyntaxFile(*primarySyntaxFile);
  //     stone::GenSyntaxFileIR(cgc, primaryFileSpecificPaths.outputFilename,
  //                            primarySyntaxFile, primaryFileSpecificPaths);
  //   }
  // }

  return Status();
}

Status CodeGenExecution::ExecuteGenNative(CodeGenContext &codeGenContext) {

  // Before we GenNative, it is possible we may not longer need the AST
  compiler.TryFreeASTContext();

  return Status();
}
