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

  llvm::PrettyStackTraceString crashInfo("Driver construction.");

  FINISH_LLVM_INIT();

  Driver driver;

  auto FinishMain = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag() : driver.GetDiags().Finish());
  };

  // Check for empty args
  if (args.empty()) {
    driver.PrintHelp();
    return FinishMain(Status::Error());
  }

  TextDiagnosticFormatter formatter;
  TextDiagnosticEmitter emitter(formatter);

  TextDiagnosticConsumer consumer(emitter);
  driver.AddDiagnosticConsumer(consumer);

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  driver.SetMainExecutablePath(mainExecutablePath);

  auto mainExecutableName = file::GetStem(mainExecutablePath);
  driver.SetMainExecutableName(mainExecutableName);

  auto argStrings = driver.ParseArgStrings(args);
  if (!argStrings) {
    return FinishMain(Status::Error());
  }
  auto status = driver.Setup(*argStrings);
  if(status.IsError()){
    return FinishMain(status);
  }
  if(status.HasCompletion()){
    return FinishMain(status);
  }

  // if (driver.GetDriverOptions().GetAction().IsSupport()) {
  //   driver.PrintSupport();
  //   return FinishMain();
  // }
  // // // Now, setup the driver
  // if (driver.Setup().IsError()) {
  //   return FinishMain(Status::Error());
  // }
  // assert(driver.HasToolChain());

  // assert(driver.HasTaskQueue());
  // assert(driver.HasCompilation());

  // if (driver.ExecuteCompilation().IsError()) {
  //   return FinishMain(Status::Error());
  // }

  // Compilation compilation(driver);
  // compilation.Setup();
  // compilation.ExecuteJobs();

  return FinishMain();
}
