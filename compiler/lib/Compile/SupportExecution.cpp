#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

PrintHelpExecution::PrintHelpExecution(Compiler &compiler,
                                       ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status PrintHelpExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::PrintHelp ||
         GetExecutionAction() == ActionKind::PrintHelpHidden);

  return Status();
}

PrintVersionExecution::PrintVersionExecution(Compiler &compiler,
                                             ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status PrintVersionExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::PrintVersion);
  return Status();
}

PrintFeatureExecution::PrintFeatureExecution(Compiler &compiler,
                                             ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status PrintFeatureExecution::Execute() {
  assert(GetExecutionAction() == ActionKind::PrintFeature);
  return Status();
}
