#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SemanticAnalysisExecution::SemanticAnalysisExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status SemanticAnalysisExecution::Setup() {

  auto execution = compiler.GetExecutionForAction(ActionKind::Parse);
  execution->Setup();
  execution->Execute();
}

Status SemanticAnalysisExecution::Execute() {

  // switch (compiler.GetInvocation().GetAction().GetKind()) {}
}

Status SemanticAnalysisExecution::ExecuteTypeCheck() {}

Status SemanticAnalysisExecution::ExecuteDumpTypeInfo() {}

Status SemanticAnalysisExecution::ExecutePrintSyntax() {}