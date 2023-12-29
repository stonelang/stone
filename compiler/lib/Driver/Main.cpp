#include "stone/Driver/Main.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Driver/Driver.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr) {

  llvm::PrettyStackTraceString crashInfo("Driver construction.");

  FINISH_LLVM_INIT();

  Driver driver;
  TextDiagnosticFormatter formatter;
  TextDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);
  driver.AddDiagnosticConsumer(consumer);

  auto FinishMain = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag() : driver.GetDiags().Finish());
  };

  // Check for empty args
  if (args.empty()) {
    driver.PrintHelp(false);
    return FinishMain(Status::Error());
  }

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  driver.GetInvocation().GetDriverOptions().mainExecutablePath =
      mainExecutablePath;

  auto mainExecutableName = file::GetStem(mainExecutablePath);
  driver.GetInvocation().GetDriverOptions().mainExecutableName =
      mainExecutableName;

  if (driver.GetInvocation().ParseCommandLine(args).IsError()) {
    return FinishMain(Status::Error());
  }

  if (!driver.GetInvocation().HasAction()) {
    // driver.GetDiags().PrintD(diag::err_no_compile_action);
    FinishMain(Status::Error());
  }

  // // Now, setup the driver
  if (driver.Setup().IsError()) {
    return FinishMain(Status::Error());
  }
  assert(driver.HasToolChain());
  assert(driver.HasTaskQueue());
  assert(driver.HasCompilation());

  if (driver.ExecuteCompilation().IsError()) {
    return FinishMain(Status::Error());
  }
  return FinishMain();
}
