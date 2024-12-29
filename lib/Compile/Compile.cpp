#include "stone/Compile/Compile.h"
#include "stone/AST/Diagnostics.h"
#include "stone/AST/DiagnosticsCompile.h"
#include "stone/AST/Module.h"
#include "stone/Basic/About.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Compile/CompilerAction.h"
#include "stone/Compile/CompilerInstance.h"
#include "stone/Compile/CompilerObservation.h"
#include "stone/Support/Statistics.h"

using namespace stone;

int stone::Compile(llvm::ArrayRef<const char *> args, const char *arg0,
                   void *mainAddr, CompilerObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Compile construction...");
  FINISH_LLVM_INIT();

  CompilerInvocation invocation;
  CompilerDiagnosticPrinter printer(invocation.GetSrcMgr());
  invocation.AddDiagnosticConsumer(printer);

  auto FinishCompile = [&](Status status = Status::Success()) -> int {
    if (invocation.GetDiags().finishProcessing()) {
      return 1;
    }
    if (status.IsError()) {
      return 1;
    }
    return 0;
  };

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  invocation.SetMainExecutablePath(mainExecutablePath);
  assert(invocation.GetCompilerOptions().HasMainExecutablePath() &&
         "Did not find an executable path!");

  auto mainExecutableName = llvm::sys::path::stem(arg0);
  invocation.SetMainExecutableName(mainExecutableName);
  assert(invocation.GetCompilerOptions().HasMainExecutableName() &&
         "Did not find an executable name!");

  if (args.empty()) {
    invocation.GetDiags().diagnose(SrcLoc(), diag::error_no_input_files);
    return FinishCompile(Status::Error());
  }

  auto status = invocation.ParseArgs(args);
  if (status.IsError()) {
    return FinishCompile(Status::Error());
  }
  printer.setFormattingStyle(invocation.GetDiagnosticOptions().formattingStyle);

  CompilerInstance compiler(invocation);

  if (!compiler.HasPrimaryAction()) {
    // compiler.GetDiags().diagnose(diag::error_no_compile_action);
    return FinishCompile(Status::Error());
  }

  if (compiler.GetInvocation().GetCompilerOptions().IsImmediateAction()) {
    compiler.ExecuteAction();
    return FinishCompile();
  }

  compiler.SetObservation(observation);
  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedCommandLineParsing(compiler);
  }
  // Now, setup the compiler
  if (!compiler.Setup()) {
    return FinishCompile(Status::Error());
  }

  if (compiler.HasObservation()) {
    compiler.GetObservation()->CompletedConfiguration(compiler);
  }
  if (!compiler.ExecuteAction()) {
    return FinishCompile(Status::Error());
  }
  return FinishCompile();
}

bool PrintHelpAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintHelpAction has to be the PrimaryAction!");

  instance.GetInvocation().GetCompilerOptions().PrintHelp();
  return true;
}
bool PrintHelpHiddenAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintHelpHiddenAction has to be the PrimaryAction!");

  instance.GetInvocation().GetCompilerOptions().PrintHelp(true);
  return true;
}
bool PrintVersionAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintVersionAction has to be the PrimaryAction!");

  return true;
}

bool PrintFeatureAction::ExecuteAction() {
  assert(GetSelfActionKind() == GetPrimaryActionKind() &&
         "PrintFeatureAction has to be the PrimaryAction!");

  return true;
}
