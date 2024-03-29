#include "stone/Driver/Main.h"

#include "stone/Basic/Defer.h"
#include "stone/Basic/LLVMInit.h"
#include "stone/Basic/MainExecutablePath.h"
#include "stone/Diag/DiagUnit.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DebugCompilationListener.h"
#include "stone/Driver/Driver.h"

using namespace stone;

int stone::Main(llvm::ArrayRef<const char *> args, const char *arg0,
                void *mainAddr, CompilationObserver *observer) {
  llvm::PrettyStackTraceString crashInfo("Driver construction.");
  FINISH_LLVM_INIT();

  DriverInvocation invocation;

  auto FinishDriver = [&](Status status = Status::Success()) -> int {
    return (status.IsError() ? status.GetFlag()
                             : invocation.GetDiags().Finish());
  };

  // std::unique_ptr<DebugCompilationListener> debugListener;

  // auto path = stone::GetMainExecutablePath(arg0);
  // auto name = file::GetStem(path);

  // Driver driver(name, path);
  // STONE_DEFER { driver.Finish(); };

  // if (args.empty()) {
  //   driver.GetLangContext().GetDiags().PrintD(SrcLoc(),
  //                                             diag::err_no_input_files);
  //   return Error(true);
  // }
  // if (listener) {
  //   driver.SetListener(listener);
  // } else {
  //   debugListener = std::make_unique<DebugCompilationListener>();
  //   driver.SetListener(debugListener.get());
  // }
  // auto ial = driver.ParseArgs(args);
  // if (!ial) {
  //   return Error(true);
  // }
  // if (driver.HasError()) {
  //   return Error(true);
  // }
  // if (driver.ComputeOptions(*ial).Has()) {
  //   return Error(true);
  // }

  // if (driver.GetDriverOptions().GetAction().IsAlien()) {
  //   driver.GetLangContext().GetDiags().PrintD(SrcLoc(),
  //   diag::err_alien_mode); return Error(true);
  // }
  // if (driver.GetDriverOptions().GetAction().IsPrintHelp()) {
  //   driver.PrintHelp(driver.GetOpts());
  //   return Error();
  // }
  // if (driver.GetDriverOptions().GetAction().IsPrintVersion()) {
  //   driver.PrintVersion();
  //   return Error();
  // }

  // auto toolChain = driver.BuildToolChain(*ial.get());
  // if (driver.HasError()) {
  //   return Error(true);
  // }
  // toolChain->Initialize();

  // if (driver.HasError()) {
  //   return Error(true);
  // }

  // auto compilation = driver.BuildCompilation(*toolChain, *ial.get());
  // if (driver.HasError()) {
  //   return Error(true);
  // }
  // if (compilation) {
  //   if (compilation->RunJobs().Has()) {
  //     return Error(true);
  //   }
  // }
  return Error();
}
