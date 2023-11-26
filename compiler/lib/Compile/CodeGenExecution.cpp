#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

// TODO: You are passing nullptr for the Hash
IRGeneration::IRGeneration(Compiler &compiler)
    : compiler(compiler),
      codeGenContext(new CodeGenContext(
          compiler.GetInvocation().GetCodeGenOptions(),
          compiler.GetInvocation().GetCompilerOptions().moduleOpts.moduleName,
          compiler.GetASTContext())) {}

Status IRGeneration::GenerateIR() {
  if (GetCodeGenContext().GetCodeGenOptions().isWholeModuleCompile) {
    return GenForWholeModule();
  } else {
    return GenForSourceFile();
  }
  //  switch (GetCodeGenContext().GetCodeGenOptions().irCodeGenTarget) {
  //  case IRCodeGenTarget::SoureFile:
  //    return GenForSourceFile();
  //  case IRCodeGenTarget::WholeModule:
  //    return GenForWholeModule();
  //  default:
  //    llvm_unreachable("Invalid IR generation kind -- only supports SourceFile
  //    "
  //                     "or WholeModule");
  //  }
}

Status IRGeneration::GenForSourceFile() {
  assert(!GetCodeGenContext().GetCodeGenOptions().isWholeModuleCompile);
  for (auto *primarySyntaxFile : compiler.GetPrimarySourceFiles()) {
    const PrimaryFileSpecificPaths primaryFileSpecificPaths =
        compiler.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
            *primarySyntaxFile);
    stone::GenSourceFileIR(GetCodeGenContext(),
                           primaryFileSpecificPaths.outputFilename,
                           primarySyntaxFile, primaryFileSpecificPaths);
  }
  return Status();
}
Status IRGeneration::GenForWholeModule() {
  assert(GetCodeGenContext().GetCodeGenOptions().isWholeModuleCompile);
  auto *mainModule = compiler.GetMainModule();
  const PrimaryFileSpecificPaths primaryFileSpecificPaths =
      compiler.GetInvocation()
          .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();
  // We take the all the files and generate a module
  stone::GenWholeModuleIR(GetCodeGenContext(),
                          primaryFileSpecificPaths.outputFilename, mainModule,
                          primaryFileSpecificPaths);
  return Status();
}

EmitIRBeforeExecution::EmitIRBeforeExecution(Compiler &compiler,
                                             ActionKind currentAction)
    : CompilerExecution(compiler, currentAction), IRGeneration(compiler) {}

Status EmitIRBeforeExecution::Execute() {
  if (GenerateIR().IsError()) {
    return Status::Error();
  }
  return Status();
}

EmitIRAfterExecution::EmitIRAfterExecution(Compiler &compiler,
                                           ActionKind currentAction)
    : CompilerExecution(compiler, currentAction), IRGeneration(compiler) {}

Status EmitIRAfterExecution::Execute() {
  if (GenerateIR().IsError()) {
    return Status::Error();
  }
  return Status();
}

EmitBitCodeExecution::EmitBitCodeExecution(Compiler &compiler,
                                           ActionKind currentAction)
    : CompilerExecution(compiler, currentAction), IRGeneration(compiler) {}

Status EmitBitCodeExecution::Execute() { return Status(); }

EmitModuleExecution::EmitModuleExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction), IRGeneration(compiler) {}

Status EmitModuleExecution::Execute() { return Status(); }

EmitNativeExecution::EmitNativeExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction), IRGeneration(compiler) {}

Status EmitNativeExecution::Execute() {
  if (GenerateIR().IsError()) {
    return Status::Error();
  }
  GetCompiler().TryFreeASTContext();

  stone::GenNative(GetCodeGenContext(),
                   GetCodeGenContext().GetLLVMModule().getName());

  return Status();
}
