#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}

void CompilerExecution::Setup() {}

Status CompilerExecution::ExecuteAction(ActionKind action) {
  currentAction = action;
  switch (currentAction) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
    return ExecutePrintHelp();
  case ActionKind::PrintVersion:
    return ExecutePrintVersion();
  case ActionKind::Parse:
    return ExecuteParseOnly();
  case ActionKind::ResolveImports:
    return ExecuteResolveImports();
  case ActionKind::DumpSyntax:
    return ExecuteDumpSyntax();
  case ActionKind::TypeCheck:
    return ExecuteTypeCheck();
  case ActionKind::PrintSyntax:
    return ExecutePrintSyntax();
  case ActionKind::MergeModules:
    return ExecuteMergeModules();
  case ActionKind::EmitIRBefore: {
    return WithCompletedTypeChecking([&]() { return ExecuteEmitIRBefore(); });
  }
  case ActionKind::EmitIRAfter: {
    return WithCompletedTypeChecking([&]() { return ExecuteEmitIRAfter(); });
  }
  case ActionKind::EmitBC: {
    return WithCompletedTypeChecking([&]() { return ExecuteEmitBC(); });
  }
  case ActionKind::None:
  case ActionKind::EmitObject: {
    return WithCompletedTypeChecking([&]() { return ExecuteEmitObject(); });
  }
  case ActionKind::DumpTypeInfo: {
    return WithCompletedTypeChecking([&]() { return ExecuteDumpTypeInfo(); });
  }
  default:
    llvm_unreachable("Unknown action -- cannot compile!");
  }
}

Status CompilerExecution::ExecuteAction() {
  compiler.ForEachAction([&](ActionKind action) {
    if (ExecuteAction(action).IsError()) {
      return Status::Error();
    }
  });
  return Status();
}
