#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/Compiler.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}

ActionKind CompilerExecution::GetMainAction() {
  compiler.GetMainAction();
}

Status CompilerExecution::SetupAction() {

  if (HasDepAction()) {
    auto execution = compiler.CreateExectution(GetDepAction());
    execution->SetConsumer(GetConsumer());
    if (compiler.ExecuteAction(*execution).IsError()) {
      return Status::MakeHasCompletionAndIsError();
    }
  }
  return Status();
}

Status CompilerExecution::FinishAction() { return Status(); }

Status Compiler::ExecuteAction(CompilerExecution &execution) {

  if (execution.SetupAction().IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }
  if (execution.ExecuteAction().IsError()) {
    return Status::MakeHasCompletionAndIsError();
  }
  return execution.FinishAction();
}

Status Compiler::ExecuteAction(ActionKind action) {
  auto execution = CreateExectution(action);
  return ExecuteAction(*execution);
}

// bool CompilerExecution::IsMainAction() {
//   return GetCurrentAction() == compiler.GetMainAction();
// }
// ActionKind CompilerExecution::GetExecutionAction() {
//   return IsMainAction() ? compiler.GetMainAction() : GetCurrentAction();
// }

// void CompilerExecution::CompletedCommandLineParsing(Compiler &compiler) {
//   llvm_unreachable("Not allowed to handle command line parsing!");
// }

// /// It is useful for the caller to return iteself because it allows use to
// /// perform a lot of checks when we are dealing with dependencies.
// CompilerExecution *CompilerExecution::GetConsumer() { return this; }

// // Some executions may want to have acccess to syntax analysis results.
// void CompilerExecution::CompletedSyntaxAnalysis(SourceFile *result) {
//   llvm_unreachable("Not allowed to handle sysntax analysis!");
// }

// // Some exection may want to have acccess to the type checking results
// void CompilerExecution::CompletedSemanticAnalysis(SourceFile *result) {
//   llvm_unreachable("Not allowed to handle semantic analysis!");
// }

// // Some exection may want to have acccess to the type checking results
// void CompilerExecution::CompletedSemanticAnalysis(ModuleDecl *result) {
//   llvm_unreachable("Not allowed to handle semantic analysis!");
// }

// /// Some executions may require access to the results of ir generation.
// void CompilerExecution::CompletedIRGeneration(
//     llvm::ArrayRef<IRGenResult *, 8> results) {
//   llvm_unreachable("Not allowed to handle IR generation!");
// }

CompilerExecution::~CompilerExecution() {}


std::unique_ptr<CompilerExecution> Compiler::CreateExectution(ActionKind kind) {
  switch (kind) {
  case ActionKind::PrintHelp:
    return std::make_unique<PrintHelpExecution>(*this);
  case ActionKind::PrintHelpHidden:
    return std::make_unique<PrintHelpHiddenExecution>(*this);
  case ActionKind::PrintVersion:
    return std::make_unique<PrintVersionExecution>(*this);
  case ActionKind::PrintFeature:
    return std::make_unique<PrintFeatureExecution>(*this);
  case ActionKind::Parse:
    return std::make_unique<ParseExecution>(*this);
  case ActionKind::DumpAST:
    return std::make_unique<DumpASTExecution>(*this);
  case ActionKind::ResolveImports:
    return std::make_unique<ImportResolutionExecution>(*this);
  case ActionKind::TypeCheck:
    return std::make_unique<TypeCheckExecution>(*this);
  case ActionKind::PrintAST:
    return std::make_unique<PrintASTExecution>(*this);
  case ActionKind::EmitIRBefore:
    return std::make_unique<GenerateIRExecution>(*this);
  case ActionKind::EmitIRAfter:
    return std::make_unique<OptimizeIRExecution>(*this);
  case ActionKind::EmitModule:
    return std::make_unique<EmitModuleExecution>(*this);
  case ActionKind::EmitBC:
    return std::make_unique<EmitBitCodeExecution>(*this);
  case ActionKind::EmitAssembly:
    return std::make_unique<EmitAssemblyExecution>(*this);
  case ActionKind::EmitObject:
    return std::make_unique<EmitObjectExecution>(*this);
  default: {
    llvm_unreachable("Unable to create CompilerExecution -- unknon action!");
  }
  }
}
