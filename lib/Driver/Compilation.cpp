#include "stone/Driver/Compilation.h"
#include "stone/Driver/BuildSystem.h"
#include "stone/Driver/CompilationListener.h"
#include "stone/Driver/Job.h"

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

Compilation::Compilation(Context &ctx, ToolChain &tc, BuildSystem &bs,
                         CompilationListener *listener)
    : ctx(ctx), tc(tc), bs(bs), listener(listener) {

  stats = std::make_unique<CompilationStats>(*this, ctx);
  ctx.GetStatEngine().Register(stats.get());

  switch (tc.GetKind()) {
  case ToolChainKind::Darwin:
    jobQueue = std::make_unique<DarwinJobQueue>(ctx);
    break;
  case ToolChainKind::Unix:
    jobQueue = std::make_unique<UnixJobQueue>(ctx);
    break;
  default:
    assert(false && "Unknown ToolChain Kind");
  }
}

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

void Compilation::PrintJobs() {
  // driver.GetContext().Out() << '\n';
  // while (!GetQueue().IsEmpty()) {
  //   auto job = GetQueue().Front();
  //   if (job) {
  //     if (driver.GetDriverOptions().printJobs) {
  //       job->PrintIntent();
  //     }
  //   }
  //   GetQueue().Pop();
  // }
}

void CompilationStats::Print() {
  if (compilation.GetContext().GetSystemOptions().printStatistics) {
    // GetContext().Out() << compilation.GetSessionName() << '\n';
  }
}
