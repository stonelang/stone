#include "stone/Driver/Run.h"

#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DiagUnit.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DebugCompilationListener.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Run(llvm::ArrayRef<const char *> args, const char *arg0,
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

  if (args.empty()) {
    driver.GetContext().GetDiagUnit().PrintD(SrcLoc(),
                                             diag::err_no_input_files);
    return Finish(1);
  }
  if (listener) {
    driver.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugCompilationListener>();
    driver.SetListener(debugListener.get());
  }
  auto ial = driver.ParseArgs(args);
  if (!ial) {
    return Finish(1);
  }
  if (driver.HasError()) {
    return Finish(1);
  }
  if (driver.ComputeOptions(*ial).Has()) {
    return Finish(1);
  }

  if (driver.GetDriverOptions().GetMode().IsAlien()) {
    driver.GetContext().GetDiagUnit().PrintD(SrcLoc(), diag::err_alien_mode);
    Finish(1);
  }
  if (driver.GetDriverOptions().GetMode().IsPrintHelp()) {
    driver.PrintHelp(driver.GetOpts());
    return Finish();
  }
  if (driver.GetDriverOptions().GetMode().IsPrintVersion()) {
    driver.PrintVersion();
    return Finish();
  }

  auto toolChain = driver.BuildToolChain(*ial.get());
  if (driver.HasError()) {
    return Finish(1);
  }
  auto compilation = driver.BuildCompilation(*toolChain, *ial.get());
  if (driver.HasError()) {
    return Finish(1);
  }
  if (compilation) {
    return Finish(compilation->RunJobs());
  }
  return Finish();
}
