#include "stone/Compile/Compile.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Compile/Compiler.h"
#include "stone/Compile/CompilerExecution.h"
#include "stone/Core.h"
#include "stone/Diag/CompilerDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Option/Action.h"
#include "stone/Stats/Stats.h"
#include "stone/Syntax/ASTDiagnosticArgument.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  ASTDiagnosticFormatter formatter;
  ASTDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);

  Compiler compiler;
  compiler.SetObservation(observation);
  compiler.AddDiagnosticConsumer(consumer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag() : compiler.GetDiags().Finish());
  };
  // Check for empty args
  if (args.empty()) {
    compiler.GetDiags().PrintD(diag::err_no_compile_args);
    return FinishCompile(Status::Error());
  }

  //  ../../stone-compile
  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  compiler.GetInvocation().GetCompilerOptions().mainExecutablePath =
      mainExecutablePath;

  // stone-compile
  auto mainExecutableName = file::GetStem(mainExecutablePath);
  compiler.GetInvocation().GetCompilerOptions().mainExecutableName =
      mainExecutableName;

  auto status = compiler.GetInvocation().ParseCommandLine(args);
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedCommandLineParsing(compiler);
  }
  if (!compiler.GetInvocation().GetCompilerOptions().HasMainAction()) {
    // compiler.GetDiags().PrintD(diag::err_no_compile_action);
    FinishCompile(Status::Error());
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
