#include "stone/Drive/Drive.h"

#include "stone/Basic/Defer.h"
#include "stone/Basic/DiagUnit.h"
#include "stone/Basic/DriverDiagnostic.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Drive/Compilation.h"
#include "stone/Drive/DebugCompilationListener.h"
#include "stone/Drive/Driver.h"

using namespace stone;

int Lang::Main(llvm::ArrayRef<const char *> args, const char *arg0,
               void *mainAddr, CompilationListener *listener) {
  llvm::PrettyStackTraceString crashInfo("Driver construction.");
  FINISH_LLVM_INIT();

  auto Error = [&](bool err = false) -> int { return 1 ? err : 0; };

  std::unique_ptr<DebugCompilationListener> debugListener;

  auto path = stone::GetMainExecutablePath(arg0);
  auto name = file::GetStem(path);

  Driver driver(name, path);
  STONE_DEFER { driver.Finish(); };

  if (args.empty()) {
    driver.GetLang().GetDiagUnit().PrintD(SrcLoc(), diag::err_no_input_files);
    return Error(true);
  }
  if (listener) {
    driver.SetListener(listener);
  } else {
    debugListener = std::make_unique<DebugCompilationListener>();
    driver.SetListener(debugListener.get());
  }
  auto ial = driver.ParseArgs(args);
  if (!ial) {
    return Error(true);
  }
  if (driver.HasError()) {
    return Error(true);
  }
  if (driver.ComputeOptions(*ial).Has()) {
    return Error(true);
  }

  if (driver.GetDriverOptions().GetMode().IsAlien()) {
    driver.GetLang().GetDiagUnit().PrintD(SrcLoc(), diag::err_alien_mode);
    return Error(true);
  }
  if (driver.GetDriverOptions().GetMode().IsPrintHelp()) {
    driver.PrintHelp(driver.GetOpts());
    return Error();
  }
  if (driver.GetDriverOptions().GetMode().IsPrintVersion()) {
    driver.PrintVersion();
    return Error();
  }

  auto toolChain = driver.BuildToolChain(*ial.get());
  if (driver.HasError()) {
    return Error(true);
  }
  toolChain->Initialize();

  if (driver.HasError()) {
    return Error(true);
  }

  auto compilation = driver.BuildCompilation(*toolChain, *ial.get());
  if (driver.HasError()) {
    return Error(true);
  }
  if (compilation) {
    if (compilation->RunJobs().Has()) {
      return Error(true);
    }
  }
  return Error();
}
