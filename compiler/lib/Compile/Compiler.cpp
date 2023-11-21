#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerInputFile.h"
#include "stone/Option/ActionKind.h"
#include "stone/Parse/Lexer.h" // TODO: do better
#include "stone/Syntax/Module.h"

using namespace stone;

Compiler::Compiler() : invocation(*this) {}

void Compiler::Setup() {
  assert(invocation.HasAction());
  mainExecution = GetExecutionForAction(invocation.GetAction().GetKind());
  mainExecution->Setup();
}

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

Status Compiler::IsValidModuleName(const llvm::StringRef moduleName) {
  if (!Lexer::isIdentifier(moduleName)) {
    return Status::Error();
  }
  return Status();
}
// void CompilerModule::AddSourceFiles() {}
// void CompilerModule::AddSourceFile() {}
