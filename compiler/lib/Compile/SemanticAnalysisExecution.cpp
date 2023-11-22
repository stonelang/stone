#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"

using namespace stone;

SemanticAnalysisExecution::SemanticAnalysisExecution(Compiler &compiler)
    : CompilerExecution(compiler) {}

Status SemanticAnalysisExecution::Execute() {

  // Everything here requires type checking -- call it now:
  if (ExecuteTypeCheck().IsError()) {
    return Status::Error();
  }

  // if(compiler.GetAction().IsTypeCheck()){
  //   return Status::Success();
  // }

  // switch (compiler.GetInvocation().GetAction().GetKind()) {
  // }
}

Status SemanticAnalysisExecution::ExecuteTypeCheck() {
  // compiler.ForEachSyntaxFile([&](SourceFile &sourceFile,
  //                       TypeCheckerOptions &typeCheckerOpts,
  //                       stone::TypeCheckerListener *listener) {
  //   stone::TypeCheckSyntaxFile(syntaxFile, typeCheckerOpts, listener);
  // });
}

Status SemanticAnalysisExecution::ExecuteDumpTypeInfo() {}

Status SemanticAnalysisExecution::ExecutePrintSyntax() {}