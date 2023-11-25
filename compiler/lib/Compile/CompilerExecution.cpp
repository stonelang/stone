#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : compiler(compiler), currentAction(currentAction) {
  assert(currentAction != ActionKind::Alien);
}
Status CompilerExecution::Setup() {
  if (HasDependency()) {
    if (compiler.ExecuteAction(GetDependency()).IsError()) {
      SetDependencyStatus(Status::Error());
      return Status::Error();
    }
    SetDependencyStatus(Status());
  }
  return Status::Success();
}
CompilerExecution::~CompilerExecution() {}

std::unique_ptr<CompilerExecution>
Compiler::GetExecutionForAction(ActionKind action) {
  switch (action) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
    return std::make_unique<PrintHelpExecution>(*this, action);
  case ActionKind::PrintVersion:
    return std::make_unique<PrintVersionExecution>(*this, action);
  case ActionKind::PrintFeature:
    return std::make_unique<PrintFeatureExecution>(*this, action);
  case ActionKind::Parse:
    return std::make_unique<ParseOnlyExecution>(*this, action);
  case ActionKind::DumpAST:
    return std::make_unique<DumpASTExecution>(*this, action);
  case ActionKind::ResolveImports:
    return std::make_unique<ImportResolutionExecution>(*this, action);
  case ActionKind::TypeCheck:
    return std::make_unique<TypeCheckExecution>(*this, action);
  case ActionKind::PrintAST:
    return std::make_unique<PrintASTExecution>(*this, action);
  case ActionKind::EmitIRBefore:
    // NOTE: This may be done in GenerateCode
    return std::make_unique<EmitIRBeforeExecution>(*this, action);
  case ActionKind::EmitIRAfter:
    return std::make_unique<EmitIRAfterExecution>(*this, action);
  case ActionKind::EmitModule:
    return std::make_unique<EmitModuleExecution>(*this, action);
  case ActionKind::EmitBC:
    return std::make_unique<EmitBitCodeExecution>(*this, action);
  case ActionKind::EmitAssembly:
  case ActionKind::EmitObject:
    return std::make_unique<EmitNativeExecution>(*this, action);
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
