#include "stone/Drive/Compilation.h"
#include "stone/Drive/CompilationListener.h"
#include "stone/Drive/DarwinTaskQueue.h"
#include "stone/Drive/Driver.h"
#include "stone/Drive/Job.h"
#include "llvm/Support/BuryPointer.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Support/Errc.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/LockFileManager.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TimeProfiler.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

using stone::Compilation;
using stone::CompilationStats;
using stone::Job;
using stone::ModeKind;

Compilation::Compilation(Driver &driver, ToolChain &tc,
                         std::unique_ptr<llvm::opt::DerivedArgList> dal)
    : driver(driver), tc(tc), dal(std::move(dal)) {

  stats = std::make_unique<CompilationStats>(*this);
  driver.GetLangContext().GetStatEngine().Register(stats.get());

  switch (tc.GetKind()) {
  case ToolChainKind::Darwin:
    tq = std::make_unique<darwin::DarwinTaskQueue>(driver.GetLangContext());
    break;
  default:
    stone::Panic("Unknown ToolChain Kind -- cannot create TaskQueue");
  }
}
stone::Error Compilation::RunJobs() {
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
  return stone::Error();
}

void Compilation::PrintJobs() {
  // driver.GetLangContext().Out() << '\n';
  // while (!GetQueue().IsEmpty()) {
  //   auto job = GetQueue().Front();
  //   if (job) {
  //     if (driver.GetDriverOptions().printJobs) {
  //       job->PrintPhase();
  //     }
  //   }
  //   GetQueue().Pop();
  // }
}

void CompilationStats::Print(ColorStream &stream) {
  if (compilation.GetDriver()
          .GetLangContext()
          .GetLangOptions()
          .printStatistics) {
    // GetLangContext().Out() << compilation.GetSessionName() << '\n';
  }
}
