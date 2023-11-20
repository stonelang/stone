#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SupportExecution::SupportExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status SupportExecution::Setup() {}

Status SupportExecution::Execute() {

  switch (compiler.GetInvocation().GetAction().GetKind()) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
    return ExecutePrintHelp();
  case ActionKind::PrintVersion:
    return ExecutePrintVersion();
  case ActionKind::PrintFeature:
    return ExecutePrintFeature();
  default:
    llvm_unreachable("Invalid action for syntax analysis");
  }
}
Status SupportExecution::ExecutePrintHelp() { return Status(); }
Status SupportExecution::ExecutePrintVersion() { return Status(); }
Status SupportExecution::ExecutePrintFeature() { return Status(); }
