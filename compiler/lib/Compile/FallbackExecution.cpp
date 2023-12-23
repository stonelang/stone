#include "stone/Basic/Status.h"
#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

Status stone::CompileForLLVMIR(Compiler &compiler) { return Status(); }

FallbackExecution::FallbackExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status FallbackExecution::Execute() {

  // We did not find a compiler action
  assert(compiler.GetMainAction() == ActionKind::None);

  if (GetCompiler().IsCompileForLLVMIR()) {
    return stone::CompileForLLVMIR(GetCompiler());
  }
  /// Perform anything that are no action related
  return Status();
}
