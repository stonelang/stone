#include "stone/Driver/Run.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMInit.h"
#include "stone/Core/MainExecutablePath.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DebugCompilationListener.h"
#include "stone/Driver/Driver.h"

using namespace stone;

// void Driver::Run() {

//   // assert(GetSessionKind() == SessionKind::Compilation && "Invalid
//   // SessionKind");

//   // if (GetDriverOptions().inputFiles.empty()) {
//   //   // GetContext().GetDiagEngine().Printd(SrcLoc(),
//   //   diag::err_no_input_files); return stone::err;
//   // }

//   // if (driverOpts.printJobs) {
//   //   GetCompilation().PrintJobs();
//   //   return stone::ok;
//   // }
//   // GetCompilation().RunJobs();
//   // return stone::ok;
// }

void Compilation::RunJobs() {
  // while (!GetQueue().IsEmpty()) {
  //   auto job = GetQueue().Front();
  //   if (job) {
  //     if (job->IsAsync()) {
  //       job->ExecuteAsync();
  //     } else {
  //       job->ExecuteSync();
  //     }
  //   }
  //   GetQueue().Pop();
  // }
}

int driver::Run(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr, CompilationListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Driver construction.");
  FINISH_LLVM_INIT();

  auto path = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto name = file::GetStem(path);

  auto Finish = [&](int status = 0) -> int {
    int err = 1;
    return status ? status : err;
  };

  if (args.empty()) {
    // ctx.Printd(SrcLoc(), diag::err_no_input_args);
    return Finish(1);
  }

  std::unique_ptr<DebugCompilationListener> debugListener;

  Driver driver(name, path);
  STONE_DEFER { driver.Finish(); };
  driver.Initialize();

  if (listener) {
    driver.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugCompilationListener>();
    driver.SetListener(debugListener.get());
  }

  auto &ial = driver.ParseArgs(args);
  if (driver.HasError()) {
    return Finish(1);
  }
  auto &mode = driver.CreateMode(ial);
  if (mode.IsAlien()) {
    // lang.Printd(SrcLoc(), diags::err_alien_mode)
    Finish(1);
  }
  if (mode.IsPrintHelp()) {
    // lang.PrintHelp();
    return Finish();
  }
  if (mode.IsPrintVersion()) {
    driver.PrintVersion();
    return Finish();
  }
  if (!mode.CanCompile()) {
    /// lang.Printd()
    return Finish(1);
  }
  auto inputs = driver.BuildInputFiles(ial);
  if (driver.HasError()) {
    return Finish(1);
  }

  // auto tc = driver.BuildToolChain(driver.GetOptUtil().GetInputArgList());
}
