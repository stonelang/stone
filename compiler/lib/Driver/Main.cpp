#include "stone/Driver/Main.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr) {

  llvm::PrettyStackTraceString crashInfo("Driver construction...");

  FINISH_LLVM_INIT();

  Driver driver;
  auto FinishMain = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag() : driver.GetDiags().Finish());
  };

  TextDiagnosticFormatter formatter;
  TextDiagnosticEmitter emitter(formatter);

  TextDiagnosticConsumer consumer(emitter);
  driver.AddDiagnosticConsumer(consumer);

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  driver.SetMainExecutablePath(mainExecutablePath);
  assert(driver.GetDriverOptions().HasMainExecutablePath() &&
         "Did not find an executable path!");

  auto mainExecutableName = llvm::sys::path::stem(arg0);
  driver.SetMainExecutableName(mainExecutableName);
  assert(driver.GetDriverOptions().HasMainExecutableName() &&
         "Did not find an executable name!");

  auto argStrings = driver.ParseArgStrings(args);
  if (!argStrings) {
    return FinishMain(Status::Error());
  }

  auto status = driver.ConvertArgStrings(*argStrings);
  status = [&](Status status) -> Status {
    if (driver.GetDriverOptions().GetInputsAndOutputs().HasNoInputs()) {
      driver.PrintHelp();
    } else if (driver.GetDriverOptions().IsHelpAction()) {
      driver.PrintHelp();
    } else if (driver.GetDriverOptions().IsHelpHiddenAction()) {
      driver.PrintHelp(true /* show hidden options*/);
    }
    return status;
  }(status);

  if (status.IsErrorOrHasCompletion()) {
    return FinishMain(status);
  }
  // Now, we can build the ToolChain
  assert(driver.GetDriverOptions().HasToolChainKind() &&
         "toolchains not found -- cannot proceed with compilation!");

  auto toolChain =
      driver.BuildToolChain(driver.GetDriverOptions().GetToolChainKind());
  if (!toolChain) {
    return FinishMain(Status::Error());
  }
  // driver.BuildCompilationEntities(*toolChain);

  // auto compileStyle = driver.BuildCompileStyle();
  // compileStyle->BuildCompilationEntities(driver.GetCompilationEntities());

  return FinishMain();
}
