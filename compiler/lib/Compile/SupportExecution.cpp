#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

PrintHelpExecution::PrintHelpExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintHelpExecution::Execute() {

  assert(IsSelfAction());
  return Status();
}

PrintHelpHiddenExecution::PrintHelpHiddenExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintHelpHiddenExecution::Execute() {

  assert(IsSelfAction());
  return Status();
}

PrintVersionExecution::PrintVersionExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintVersionExecution::Execute() {

  assert(IsSelfAction());
  return Status();
}

PrintFeatureExecution::PrintFeatureExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status PrintFeatureExecution::Execute() {
  assert(IsSelfAction());
  return Status();
}
