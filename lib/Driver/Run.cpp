#include "stone/Driver/Run.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMInit.h"
#include "stone/Core/MainExecutablePath.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using stone::Driver;

int Driver::Run() {

  llvm::PrettyStackTraceString crashInfo("Driver construction...");
  // assert(GetSessionKind() == SessionKind::Compilation && "Invalid
  // SessionKind");

  // if (GetDriverOptions().inputFiles.empty()) {
  //   // GetContext().GetDiagEngine().Printd(SrcLoc(),
  //   diag::err_no_input_files); return stone::err;
  // }

  if (driverOpts.printJobs) {
    GetCompilation().PrintJobs();
    return stone::ok;
  }
  GetCompilation().RunJobs();
  return stone::ok;
}

int stone::Run(llvm::ArrayRef<const char *> args, const char *arg0,
               void *mainAddr, CompilationListener *listener) {

  FINISH_LLVM_INIT();

  auto programPath = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto programName = file::GetStem(programPath);

  Driver driver(programName.data(), programPath.c_str());
  driver.Initialize();
  // driver.SetListener(listener);

  STONE_DEFER { driver.Finish(); };
  driver.Build(args);
  return driver.Run();
}
