#include "stone/Compile/Compile.h"
#include "stone/Compile/Compiler.h"
#include "stone/Option/ActionKind.h"
#include "stone/Public.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *listener) {

  Compiler compiler;

  compiler.GetInvocation().ParseCommandLine(args);

  // if(!compiler.GetInvocation().HasAction()){
  // }

  compiler.Setup();

  // compiler.GetExecution().ExecuteAction();

  return 0;
}

// Status CompilerExecution::ExecutionAction() {
//   // compiler.ForEachAction([](ActionKind action)) { ExecutionAction(action);
//   }
// }

Status CompilerExecution::ExecutionAction(ActionKind action) {
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
  case ActionKind::EmitIRBefore:
    return ExecuteEmitIRBefore();
  case ActionKind::EmitIRAfter:
    return ExecuteEmitIRAfter();
  case ActionKind::EmitBC:
    return ExecuteEmitBC();
  case ActionKind::EmitObject:
    return ExecuteEmitObject();
  case ActionKind::DumpTypeInfo:
    return ExecuteDumpTypeInfo();
  default:
    llvm_unreachable("Unknown action -- cannot compile!");
  }
}

Status CompilerExecution::ExecuteParseOnly() {
  assert(currentAction == ActionKind::Parse);
  CompilerExecutionRAII compilerExectutionRAII(*this);

  return Status();
}
Status CompilerExecution::ExecuteResolveImports() {
  assert(currentAction == ActionKind::ResolveImports);

  CompilerExecutionRAII exectutionRAII(*this);
  return Status();
}

Status CompilerExecution::ExecuteTypeCheck() {
  assert(currentAction == ActionKind::TypeCheck);
  assert(GetStages().HasCompletedSyntaxAnalysis());

  GetStages().AddCompletedSyntaxAnalysis();
  CompilerExecutionRAII exectutionRAII(*this);

  return Status();
}

Status CompilerExecution::ExecuteDumpSyntax() {
  CompilerExecutionRAII exectutionRAII(*this);
  return Status();
}

Status CompilerExecution::ExecuteEmitObject() {
  assert(currentAction == ActionKind::EmitObject);
  assert(GetStages().HasCompletedSemanticAnalysis());

  CompilerExecutionRAII exectutionRAII(*this);

  ExecuteGenerateIR();

  return Status();
}

CompilerExecutionRAII::CompilerExecutionRAII(CompilerExecution &execution)
    : execution(execution) {}

CompilerExecutionRAII::~CompilerExecutionRAII() {
  /// Print out the time
}
