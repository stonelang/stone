#include "stone/Driver/Main.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr) {

  llvm::PrettyStackTraceString crashInfo("Driver construction.");

  FINISH_LLVM_INIT();

  Driver driver;

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  driver.GetDriverOptions().mainExecutablePath = mainExecutablePath;

  auto mainExecutableName = file::GetStem(mainExecutablePath);
  driver.GetDriverOptions().mainExecutableName = mainExecutableName;

  auto inputArgList = driver.ParseCommandLine(args);
  if(!inputArgList){
    return 1; 
  }
  auto derivedArgList = driver.TranslateInputArgList(*inputArgList);

  driver.ParseDriverOptions(*inputArgList);

  return 0;
}
