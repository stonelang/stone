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
  default:
    return WithCompletedTypeChecking();
  }
}

Status CompilerExecution::WithCompletedTypeChecking() {


  assert(GetStages().DidFinishTypeChecking());

  // CodeGenContext codeGenContext(
  //     compiler.GetInvocation().GetCodeGenOptions(),
  //     compiler.GetModuleOptions(),
  //     compiler.GetInvocation().GetTargetOptions(), compiler.GetLangContext(),
  //     compiler.GetClangContext());

  // auto status = GenerateIR(codeGenContext);
  // if(status.IsError()){
  //   status.SetHasCompletion();
  //   return status;
  // }
  // switch (currentAction) {
  // case ActionKind::EmitIRBefore:
  //   return ExecuteEmitIRBefore(codeGenContext);
  // case ActionKind::EmitIRAfter:
  //   return ExecuteEmitIRAfter(codeGenContext);
  // case ActionKind::EmitBC:
  //   return ExecuteEmitBC(codeGenContext);
  // case ActionKind::None:
  // case ActionKind::EmitObject:
  //   return ExecuteEmitObject(codeGenContext);
  // case ActionKind::DumpTypeInfo:
  //   return ExecuteDumpTypeInfo(codeGenContext);
  // default:
  //   llvm_unreachable("Unknown action -- cannot compile!");
  // }

  return Status();
}

Status CompilerExecution::ExecuteAction() {
  compiler.ForEachAction([&](ActionKind action) {
    if (ExecuteAction(action).IsError()) {
      return Status::Error();
    }
  });
  return Status();
}
