#include "stone/Driver/Run.h"
#include "stone/Core/Defer.h"
#include "stone/Core/LLVMInit.h"
#include "stone/Core/MainExecutablePath.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DebugCompilationListener.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int Compilation::RunJobs() {
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
  return 0;
}

int driver::Run(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr, CompilationListener *listener) {

  llvm::PrettyStackTraceString crashInfo("Driver construction.");
  FINISH_LLVM_INIT();

  auto Finish = [&](int status = 0) -> int {
    int err = 1;
    return status ? status : err;
  };

  std::unique_ptr<DebugCompilationListener> debugListener;

  auto path = llvm::sys::fs::getMainExecutable(arg0, mainAddr);
  auto name = file::GetStem(path);

  Driver driver(name, path);
  STONE_DEFER { driver.Finish(); };
  driver.Initialize();

  if (args.empty()) {
    // driver.GetContext().Printd(diag::err_no_input_args);
    return Finish(1);
  }
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
  auto &mode = driver.ComputeMode(ial);
  if (mode.IsAlien()) {
    // driver.GetContext().Printd(diags::err_alien_mode)
    Finish(1);
  }
  if (mode.IsPrintHelp()) {
    driver.PrintHelp(driver.GetOpts());
    return Finish();
  }
  if (mode.IsPrintVersion()) {
    driver.PrintVersion();
    return Finish();
  }

  auto toolChain = driver.BuildToolChain(ial);
  if (driver.HasError()) {
    return Finish(1);
  }
  auto compilation = driver.BuildCompilation(*toolChain, ial);
  if (driver.HasError()) {
    return Finish(1);
  }
  if (compilation) {
    return Finish(compilation->RunJobs());
  }
  return Finish();
}
