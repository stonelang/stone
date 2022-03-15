#include "stone/Driver/Run.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMInit.h"
#include "stone/Core/MainExecutablePath.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

void Driver::Run() {

  // assert(GetSessionKind() == SessionKind::Compilation && "Invalid
  // SessionKind");

  // if (GetDriverOptions().inputFiles.empty()) {
  //   // GetContext().GetDiagEngine().Printd(SrcLoc(),
  //   diag::err_no_input_files); return stone::err;
  // }

  // if (driverOpts.printJobs) {
  //   GetCompilation().PrintJobs();
  //   return stone::ok;
  // }
  // GetCompilation().RunJobs();
  // return stone::ok;
}

int driver::Run(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr, CompilationListener *listener) {

  FINISH_LLVM_INIT();

  llvm::PrettyStackTraceString crashInfo("Driver construction.");

  auto path = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto name = file::GetStem(programPath);

  auto Finish = [&](int status = 0) -> int {
    int err = 1;
    return status ? status : err;
  };

  if (args.empty()) {
    // ctx.Printd(SrcLoc(), diag::err_no_input_args);
    return Finish(1);
  }

  Driver driver(name, path, listener);
  driver.Initialize();

  STONE_DEFER { driver.Finish(); };

  if (driver.ParseArgs(args) == stone::Err) {
    return Finish(1);
  }
  if (driver.GetMode().IsAlien()) {
    // lang.Printd(SrcLoc(), diags::err_alien_mode)
    Finish(1);
  }

  if (driver.GetMode().IsPrintHelp()) {
    driver.PrintHelp();
    return Finish();
  }
  if (driver.GetMode().IsPrintVersion()) {
    driver.PrintVersion();
    return Finish();
  }

  auto tc = driver.BuildToolChain(driver.GetInputArgList());
}
