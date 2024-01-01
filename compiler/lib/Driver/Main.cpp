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

  auto mainExecutableName = file::GetStem(mainExecutablePath);
  driver.SetMainExecutableName(mainExecutableName);

   auto argStrings = driver.ParseArgStrings(args);
  if (!argStrings) {
    return FinishMain(Status::Error());
  }
  // Future:
  auto status = driver.ConvertArgStrings(*argStrings);
  if (status.IsErrorOrHasCompletion()) {
    return FinishMain(status);
  }

  return FinishMain();
}
