#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

FallbackExecution::FallbackExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status FallbackExecution::Execute() {

  /// Perform anything that are no action related
  return Status();
}
