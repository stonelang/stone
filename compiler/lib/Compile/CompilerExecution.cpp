#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}

Status CompilerExecution::Setup() {
  assert(GetDependency() == ActionKind::Alien);
  switch (GetDependency()) {
  case ActionKind::None:
    return Status::Success();
  default: {
    return compiler.ExecuteAction(GetDependency());
  }
  }
}
CompilerExecution::~CompilerExecution() {}

std::unique_ptr<CompilerExecution>
Compiler::GetExecutionForAction(ActionKind action) {
  switch (action) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
  case ActionKind::PrintVersion:
    return std::make_unique<SupportExecution>(*this);
  case ActionKind::Parse:
  case ActionKind::ResolveImports:
  case ActionKind::DumpSyntax:
    return std::make_unique<SyntaxAnalysisExecution>(*this);
  case ActionKind::TypeCheck:
  case ActionKind::PrintSyntax:
  case ActionKind::DumpTypeInfo:
    return std::make_unique<SemanticAnalysisExecution>(*this);
  case ActionKind::EmitIRBefore:
  case ActionKind::EmitIRAfter:
  case ActionKind::EmitBC:
  case ActionKind::EmitAssembly:
  case ActionKind::EmitObject:
    return std::make_unique<CodeGenExecution>(*this);
  default: {
    return std::make_unique<FallbackExecution>(*this);
  }
  }
}

Status Compiler::ExecuteAction(ActionKind kind) {
  auto execution = GetExecutionForAction(kind);
  if (execution->Setup().IsError()) {
    return Status::Error();
  }
  return execution->Execute();
}

void CompilerExecution::Finish() {}
