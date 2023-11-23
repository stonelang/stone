#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SemanticAnalysisExecution::SemanticAnalysisExecution(Compiler &compiler,
                                                     ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SemanticAnalysisExecution::Execute() {

  switch (GetExecutionAction()) {}
}

Status SemanticAnalysisExecution::ExecuteTypeCheck() {

  // compiler.ForEachSyntaxFile([&](SourceFile &sourceFile,
  //                       TypeCheckerOptions &typeCheckerOpts,
  //                       stone::TypeCheckerListener *listener) {
  //   stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);
  // });
}

Status SemanticAnalysisExecution::ExecutePrintAST() { return Status(); }