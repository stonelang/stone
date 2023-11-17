#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Compile/Compiler.h"
#include "stone/Option/ActionKind.h"
#include "stone/Public.h"

#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Syntax/ASTDiagnosticArgument.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  ASTDiagnosticFormatter formatter;
  ASTDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);

  Compiler compiler;
  compiler.AddDiagnosticConsumer(consumer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag() : compiler.GetDiags().Finish());
  };

  // Check for empty args
  if (args.empty()) {
    compiler.GetDiags().PrintD(diag::err_no_compile_args);
    return FinishCompile(Status::Error());
  }

  compiler.GetInvocation().ParseCommandLine(args);
  compiler.Setup();

  // compiler.GetExecution().ExecuteAction();

  return FinishCompile();
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

Status CompilerExecution::ExecutePrintHelp() { return Status(); }
Status CompilerExecution::ExecutePrintVersion() { return Status(); }
Status CompilerExecution::ExecutePrintFeature() { return Status(); }

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

Status CompilerExecution::ExecuteDumpSyntax() {
  CompilerExecutionRAII exectutionRAII(*this);
  return Status();
}

Status CompilerExecution::ExecuteTypeCheck() {
  assert(currentAction == ActionKind::TypeCheck);
  assert(GetStages().HasCompletedSyntaxAnalysis());

  GetStages().AddCompletedSemanticAnalysis();
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

Status CompilerExecution::ExecuteDumpTypeInfo() { return Status(); }
Status CompilerExecution::ExecutePrintSyntax() { return Status(); }
Status CompilerExecution::ExecutePrintIR() { return Status(); }
Status CompilerExecution::ExecuteEmitIRBefore() { return Status(); }

Status CompilerExecution::ExecuteGenerateIR() { return Status(); }
Status CompilerExecution::ExecuteInitModule() { return Status(); }
Status CompilerExecution::ExecuteEmitModule() { return Status(); }
Status CompilerExecution::ExecuteMergeModules() { return Status(); }

Status CompilerExecution::ExecuteEmitIRAfter() { return Status(); }
Status CompilerExecution::ExecuteEmitBC() { return Status(); }

Status CompilerExecution::ExecuteEmitLibrary() { return Status(); }
Status CompilerExecution::ExecuteEmitAssembly() { return Status(); }

CompilerExecutionRAII::CompilerExecutionRAII(CompilerExecution &execution)
    : execution(execution) {}

CompilerExecutionRAII::~CompilerExecutionRAII() {
  /// Print out the time
}
