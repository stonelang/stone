#include "stone/Compile/Compile.h"
#include "stone/AST/ASTDiagnosticArgument.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Core.h"
#include "stone/Support/CompilerDiagnostic.h"
#include "stone/Support/StatsReporter.h"
#include "stone/Support/TextDiagnosticConsumer.h"
#include "stone/Support/TextDiagnosticFormatter.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  ASTDiagnosticFormatter formatter;
  ASTDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);

  CompilerInvocation invocation;
  invocation.AddDiagnosticConsumer(consumer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag()
                             : invocation.GetDiags().Finish());
  };

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  invocation.SetMainExecutablePath(mainExecutablePath);
  assert(invocation.GetCompilerOptions().HasMainExecutablePath() &&
         "Did not find an executable path!");

  auto mainExecutableName = llvm::sys::path::stem(arg0);
  invocation.SetMainExecutableName(mainExecutableName);
  assert(invocation.GetCompilerOptions().HasMainExecutableName() &&
         "Did not find an executable name!");

  auto status = invocation.ParseCommandLine(args);
  if (status.IsError()) {

    // Check that this is a help request
    // if (!compiler.GetInvocation().GetCompilerOptions().IsPrintHelpAction() &&
    //     !compiler.GetInvocation()
    //          .GetCompilerOptions()
    //          .IsPrintHelpHiddenAction()) {
    //   return FinishCompile(Status::Error());
    // }
    return FinishCompile(Status::Error());
  }

  if (!invocation.GetCompilerOptions().HasMainAction()) {
    // compiler.GetDiags().PrintD(diag::err_no_compile_action);
    return FinishCompile(Status::Error());
  }

  Compiler compiler(invocation);
  compiler.SetObservation(observation);

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedCommandLineParsing(compiler);
  }

  // Now, setup the compiler
  if (compiler.Setup().IsError()) {
    return FinishCompile(Status::Error());
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedConfiguration(compiler);
  }

  if (compiler.IsLLVMCompile()) {
    if (stone::CompileLLVM(compiler).IsError()) {
      return FinishCompile(Status::Error());
    }
  }
  if (stone::CompileAction(compiler).IsError()) {
    return FinishCompile(Status::Error());
  }

  return FinishCompile();
}