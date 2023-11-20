#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

CodeGenExecution::CodeGenExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status CodeGenExecution::Setup() {
  // auto execution = compiler.GetExecution(ActionKind::TypeCheck);
  // execution->Setup();
  // execution->Execute();
}

Status CodeGenExecution::Execute() {}
