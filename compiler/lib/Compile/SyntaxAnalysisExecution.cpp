#include "stone/Basic/Status.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Public.h"

using namespace stone;

SyntaxAnalysisExecution::SyntaxAnalysisExecution(Compiler &compiler,
                                                 ActionKind currentAction)
    : CompilerExecution(compiler, currentAction) {}

Status SyntaxAnalysisExecution::Execute() {

  // switch (GetExecutionAction()) {
  // case ActionKind::Parse:
  //   return ExecuteParse(
  //       [&](SourceFile &sourceFile) { return Status::Success(); });
  // case ActionKind::ResolveImports:
  //   return ExecuteParse([&](SourceFile &sourceFile) {
  //     return ExecuteResolveImports(sourceFile)
  //   });
  // case ActionKind::DumpSyntax:
  //   return ExecuteParse([&](SourceFile &sourceFile) {
  //     return ExecutDumpSyntax(sourceFile)
  //   });
  // default:
  //   llvm_unreachable("Invalid action for syntax analysis");
  // }
}

Status SyntaxAnalysisExecution::ExecuteParse(
    std::function<Status(SourceFile &sourceFile)> notify) {

  // for (auto moduleFile : compiler.GetMainModule()->GetFiles()) {
  //   if (auto *sourceFile = llvm::dyn_cast<SourceFile>(moduleFile)) {
  //     stone::ParseSourceFile(*sourceFile, compiler.GetASTContext(), nullptr,
  //                            nullptr);
  //     if (notify) {
  //       notify(*sourceFile);
  //     }
  //   }
  // }
  // ForEachSourceFileInMainModule([](SourceFile &SF) {
  //     stone::ParseSourceFile(*sourceFile, compiler.GetASTContext(), nullptr,
  //                            nullptr);
  //     return false;
  //   });

  return Status();
}

Status SyntaxAnalysisExecution::ExecuteResolveImports(SourceFile &sourceFile) {
  return Status();
}

Status SyntaxAnalysisExecution::ExecutDumpSyntax(SourceFile &sourceFile) {
  return Status();
}
