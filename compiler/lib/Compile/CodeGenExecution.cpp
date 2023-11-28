#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/IRCodeGenRequest.h"

using namespace stone;

// IRGenCodeOuput IRGeneration::ExecuteGenIR(Compiler &compiler) {
//   if (compiler.GetInvocation().GetCodeGenOptions().isWholeModuleCompile) {
//     return stone::GenIR(CodeGenContext::ForModule())
//   }
//   return stone::GenIR(CodeGenContext::ForFile());
// }

// Status IRGeneration::GenForSourceFile() {
//   assert(!GetCodeGenContext().GetCodeGenOptions().isWholeModuleCompile);

//   stone::GenIR(CodeGenContext::ForFile())

//   // for (auto *primarySyntaxFile : compiler.GetPrimarySourceFiles()) {
//   //   const PrimaryFileSpecificPaths primaryFileSpecificPaths =
//   //       compiler.GetInvocation().GetPrimaryFileSpecificPathsForSyntaxFile(
//   //           *primarySyntaxFile);

//   //   stone::GenerateIR(GetCodeGenContext(),
//   //                     primaryFileSpecificPaths.outputFilename,
//   //                     primarySyntaxFile, primaryFileSpecificPaths);
//   // }
//   return Status();
// }
// Status IRGeneration::GenForWholeModule() {
//   assert(GetCodeGenContext().GetCodeGenOptions().isWholeModuleCompile);
//    stone::GenIR(CodeGenContext::ForModule())

//   // auto *mainModule = compiler.GetMainModule();
//   // const PrimaryFileSpecificPaths primaryFileSpecificPaths =
//   //     compiler.GetInvocation()
//   //         .GetPrimaryFileSpecificPathsForWholeModuleOptimizationMode();
//   // // We take the all the files and generate a module
//   // stone::GenerateIR(GetCodeGenContext(),
//   //                   primaryFileSpecificPaths.outputFilename, mainModule,
//   //                   primaryFileSpecificPaths);
//   return Status();
// }

GenerateIRExecution::GenerateIRExecution(Compiler &compiler,
                                         ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status GenerateIRExecution::Execute() {

  assert(GetExecutionAction() == ActionKind::EmitIRBefore);
  assert(GetDependencyStatus().IsSuccess());

  // auto codeGenResult = stone::GenIR(IRCodeGenRequest::ForFile((....)));
  // compiler.SetIRCodeGenResult(codeGenResult);

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
