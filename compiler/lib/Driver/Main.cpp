#include "stone/Driver/Main.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Diag/TextDiagnosticConsumer.h"
#include "stone/Diag/TextDiagnosticFormatter.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr) {

  llvm::PrettyStackTraceString crashInfo("Driver construction.");

  FINISH_LLVM_INIT();

  DriverInvocation invocation;
  TextDiagnosticFormatter formatter;
  TextDiagnosticEmitter emitter(formatter);
  TextDiagnosticConsumer consumer(emitter);
  invocation.AddDiagnosticConsumer(consumer);

  auto FinishMain = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag()
                             : invocation.GetDiags().Finish());
  };

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  invocation.SetMainExecutablePath(mainExecutablePath);

  auto mainExecutableName = file::GetStem(mainExecutablePath);
  invocation.SetMainExecutableName(mainExecutableName);

  if (invocation.ParseCommandLine(args).IsError()) {
    return FinishMain(Status::Error());
  }

  Driver driver(invocation);

  // Check for empty args
  // if (args.empty()) {
  //   driver.PrintHelp(false);
  //   return FinishMain(Status::Error());
  // }

  // // Now, setup the driver
  if (driver.Setup().IsError()) {
    return FinishMain(Status::Error());
  }
  // assert(driver.HasToolChain());

  // assert(driver.HasTaskQueue());
  // assert(driver.HasCompilation());

  // if (driver.ExecuteCompilation().IsError()) {
  //   return FinishMain(Status::Error());
  // }
  return FinishMain();
}
