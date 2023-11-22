#include "stone/Public.h"

#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

FallbackExecution::FallbackExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status FallbackExecution::Execute() {

  // We did not find a compiler action
  assert(GetMainAction() == ActionKind::None);

  if (compiler.GetInvocation()
          .GetCompilerOptions()
          .inputsAndOutputs.ShouldTreatAsLLVM()) {
    return ExecuteCompileLLVMIR();
  }
  /// Perform anything that are no action related
  return Status::Error();
}

Status FallbackExecution::ExecuteCompileLLVMIR() {
  // todo: stone::GenNative(.....)
  return Status();
}
