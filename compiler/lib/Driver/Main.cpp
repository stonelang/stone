#include "stone/Driver/Main.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverExecution.h"
#include "stone/Driver/DriverInvocation.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr, DriverObservation *observation) {

  llvm::PrettyStackTraceString crashInfo("Driver construction.");
  FINISH_LLVM_INIT();

  // DriverInvocation invocation;

  // auto FinishDriver = [&](Status status = Status::Success()) -> int {
  //   return (status.IsError() ? status.GetFlag()
  //                            : invocation.GetDiags().Finish());
  // };

  return 0;
}
