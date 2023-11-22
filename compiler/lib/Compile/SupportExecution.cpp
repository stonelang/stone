#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SupportExecution::SupportExecution(Compiler &compiler, ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SupportExecution::Execute() {

  switch (GetExecutionAction()) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
    return ExecutePrintHelp();
  case ActionKind::PrintVersion:
    return ExecutePrintVersion();
  case ActionKind::PrintFeature:
    return ExecutePrintFeature();
  default:
    llvm_unreachable("Invalid action for support");
  }
}
Status SupportExecution::ExecutePrintHelp() { return Status(); }
Status SupportExecution::ExecutePrintVersion() { return Status(); }
Status SupportExecution::ExecutePrintFeature() { return Status(); }
