#include "stone/Driver/Main.h"
#include "stone/AST/DiagnosticsDriver.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr) {

  llvm::PrettyStackTraceString crashInfo("Driver construction...");
  FINISH_LLVM_INIT();

  TextDiagnosticPrinter diagnosticPrinter;
  Driver driver;

  driver.AddDiagnosticConsumer(diagnosticPrinter);

  auto mainExecutablePath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  driver.SetMainExecutablePath(mainExecutablePath);
   
  auto mainExecutableName = llvm::sys::path::stem(arg0);
  driver.SetMainExecutableName(mainExecutableName);
  
}
