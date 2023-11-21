#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

CodeGenExecution::CodeGenExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status CodeGenExecution::Setup() {

  if (compiler.ExecuteAction(ActionKind::TypeCheck).IsError()) {
    return Status::Error();
  }
  return Status();
}

Status CodeGenExecution::Execute() {

  switch (compiler.GetInvocation().GetAction().GetKind()) {
  default:
    llvm_unreachable("Invalid action for CodeGeneration");
  }
}

Status CodeGenExecution::ExecuteGenIR(CodeGenContext &codeGenContext) {
  return Status();
}

Status CodeGenExecution::ExecuteGenNative(CodeGenContext &codeGenContext) {

  return Status();
}
