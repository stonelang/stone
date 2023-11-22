#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

CodeGenExecution::CodeGenExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status CodeGenExecution::Execute() {

  // Everything here requires GenIR -- call it now
  // ExecuteGenIR();

  switch (compiler.GetInvocation().GetAction().GetKind()) {
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

  return Status();
}
