#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : compiler(compiler), currentAction(currentAction) {
  assert(currentAction != ActionKind::Alien);
}
Status CompilerExecution::Setup() {
  if (GetDependency() == ActionKind::None) {
    return Status::Success();
  }
  return compiler.ExecuteAction(GetDependency());
}
CompilerExecution::~CompilerExecution() {}

std::unique_ptr<CompilerExecution>
Compiler::GetExecutionForAction(ActionKind action) {
  switch (action) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
  case ActionKind::PrintVersion:
    return std::make_unique<SupportExecution>(*this, action);
  case ActionKind::Parse:
  case ActionKind::DumpAST:
  case ActionKind::ResolveImports:
    return std::make_unique<SyntaxAnalysisExecution>(*this, action);
  case ActionKind::TypeCheck:
  case ActionKind::PrintAST:
    return std::make_unique<SemanticAnalysisExecution>(*this, action);
  case ActionKind::EmitIRBefore:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitBC:
  case ActionKind::EmitModule:
  case ActionKind::EmitAssembly:
  case ActionKind::EmitObject:
    return std::make_unique<CodeGenExecution>(*this, action);
  default: {
    return std::make_unique<FallbackExecution>(*this, action);
  }
  }
}
Status Compiler::ExecuteAction(ActionKind kind) {
  auto execution = GetExecutionForAction(kind);
  if (execution->Setup().IsError()) {
    return Status::Error();
  }
  if (execution->Execute().IsError()) {
    return Status::Error();
  }
  return execution->Finish();
}

Status CompilerExecution::Finish() { return Status(); }

ActionKind CompilerExecution::GetMainAction() {
  return compiler.GetInvocation().GetMainAction().GetKind();
}
