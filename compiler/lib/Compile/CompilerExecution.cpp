#include "stone/Compile/CompilerExecution.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerObservation.h"

using namespace stone;

CompilerExecution::CompilerExecution(Compiler &compiler) : compiler(compiler) {}
CompilerExecution::~CompilerExecution() {}

void CompilerExecution::CompletedCommandLineParsing(Compiler &compiler) {
  llvm_unreachable("Illegal to handle command line parsing!");
}
void CompilerExecution::CompletedConfiguration(Compiler &compiler) {
  llvm_unreachable("Illegal to handle compiler configuration!");
}
void CompilerExecution::CompletedSyntaxAnalysis(Compiler &result) {
  llvm_unreachable("Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedSyntaxAnalysis(SourceFile &result) {
  llvm_unreachable("Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedSyntaxAnalysis(ModuleDecl &result) {
  llvm_unreachable("Illegal to handle syntax analysis!");
}
void CompilerExecution::CompletedSemanticAnalysis(Compiler &result) {
  llvm_unreachable("Illegal to handle semantic analysis!");
}
void CompilerExecution::CompletedSemanticAnalysis(SourceFile &result) {
  llvm_unreachable("Illegal to handle semantic analysis!");
}
void CompilerExecution::CompletedSemanticAnalysis(ModuleDecl &result) {
  llvm_unreachable("Illegal to handle semantic analysis!");
}
void CompilerExecution::CompletedIRGeneration(Compiler &result) {
  llvm_unreachable("Illegal to handle IR generation!");
}
void CompilerExecution::CompletedIRGeneration(llvm::Module *result) {
  llvm_unreachable("Illegal to handle IR generation!");
}
/// Some executions may require access to the results of ir generation.
void CompilerExecution::CompletedIRGeneration(
    llvm::ArrayRef<llvm::Module *> &results) {
  llvm_unreachable("Illegal to handle IR generation!");
}
CodeCompletionCallbacks *CompilerExecution::GetCodeCompletionCallbacks() {
  llvm_unreachable("Illegal to handle code completion callbacks!");
}

ActionKind CompilerExecution::GetMainAction() { compiler.GetMainAction(); }

Compiler &CompilerExecution::GetCompiler() { return compiler; }

CompilerExecution *CompilerExecution::GetConsumer() {
  return (consumer != nullptr) ? consumer : this;
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
    return std::make_unique<EmitIRBeforeExecution>(*this);
  case ActionKind::EmitIRAfter:
    return std::make_unique<EmitIRAfterExecution>(*this);
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
