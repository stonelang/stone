#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler,
                                     ActionKind currentAction)
    : compiler(compiler), currentAction(currentAction) {
  assert(currentAction != ActionKind::Alien);
}
Status CompilerExecution::Setup() { return ExecuteDependency(); }

Status CompilerExecution::ExecuteDependency() {
  if (HasDependency()) {
    if (compiler.ExecuteAction(GetDependency()).IsError()) {
      return Status::Error();
    }
  }
  return Status::Success();
}

bool CompilerExecution::IsMainAction() {
  return GetCurrentAction() == compiler.GetMainAction();
}
ActionKind CompilerExecution::GetExecutionAction() {
  return IsMainAction() ? compiler.GetMainAction() : GetCurrentAction();
}

Status CompilerExecution::Finish() { return Status(); }

CompilerExecution::~CompilerExecution() {}

std::unique_ptr<CompilerExecution>
Compiler::ComputeCompilerExectution(ActionKind kind) {
  switch (kind) {
  case ActionKind::PrintHelp:
  case ActionKind::PrintHelpHidden:
    return std::make_unique<PrintHelpExecution>(*this, kind);
  case ActionKind::PrintVersion:
    return std::make_unique<PrintVersionExecution>(*this, kind);
  case ActionKind::PrintFeature:
    return std::make_unique<PrintFeatureExecution>(*this, kind);
  case ActionKind::Parse:
    return std::make_unique<ParseOnlyExecution>(*this, kind);
  case ActionKind::DumpAST:
    return std::make_unique<DumpASTExecution>(*this, kind);
  case ActionKind::ResolveImports:
    return std::make_unique<ImportResolutionExecution>(*this, kind);
  case ActionKind::TypeCheck:
    return std::make_unique<TypeCheckExecution>(*this, kind);
  case ActionKind::PrintAST:
    return std::make_unique<PrintASTExecution>(*this, kind);
  case ActionKind::EmitIRBefore:
    return std::make_unique<GenerateIRExecution>(*this, kind);
  case ActionKind::EmitIRAfter:
    return std::make_unique<OptimizeIRExecution>(*this, kind);
  case ActionKind::EmitModule:
    return std::make_unique<EmitModuleExecution>(*this, kind);
  case ActionKind::EmitBC:
    return std::make_unique<EmitBitCodeExecution>(*this, kind);
  case ActionKind::EmitAssembly:
  case ActionKind::EmitObject:
    return std::make_unique<EmitNativeExecution>(*this, kind);
  default: {
    return std::make_unique<FallbackExecution>(*this, kind);
  }
  }
}
Status Compiler::ExecuteAction(ActionKind kind) {
  auto execution = ComputeCompilerExectution(kind);
  if (execution->Setup().IsError()) {
    return Status::Error();
  }
  if (execution->Execute().IsError()) {
    return Status::Error();
  }
  return execution->Finish();
}
