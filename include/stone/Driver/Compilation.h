#ifndef STONE_DRIVER_COMPILATION_H
#define STONE_DRIVER_COMPILATION_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Intent.h"
#include "stone/Driver/JobQueue.h"
#include "stone/Driver/ToolChain.h"

#include "stone/Driver/CompilationJob.h"

namespace stone {

class Job;
class JobQueue;
class BuildSystem;
class Compilation;
class CompilationListener;

class CompilationStats final : public Stats {
  Compilation &compilation;

public:
  CompilationStats(Compilation &compilation, Context &ctx)
      : Stats("Compilation statistics:", ctx), compilation(compilation) {}
  void Print() override;
};

class Compilation final {
  friend CompilationStats;

  std::unique_ptr<CompilationStats> stats;
  std::unique_ptr<JobQueue> jobQueue;

  Driver &driver;
  ToolChain &tc;
  std::unique_ptr<llvm::opt::DerivedArgList> dal;

  /// The Jobs which will be performed by this compilation.
  llvm::SmallVector<std::unique_ptr<const Job>, 32> jobs;

public:
  Compilation(Driver &driver, ToolChain &tc,
              std::unique_ptr<llvm::opt::DerivedArgList> dal);

public:
  /// TODO: cleanup
  // template <typename I, typename... Args> I *CreateIntent(Args &&...args) {
  //   I *result = nullptr;
  //   auto i = std::make_unique<I>(std::forward<Args>(args)...);
  //   result = i.get();
  //   intents.Add(std::move(i));
  //   return result;
  // }
  // template <typename I, typename... Args> I *CreateIntent(Args &&...args) {
  //   auto result = new I(std::forward<Args>(args)...);
  //   intents.emplace_back(result);
  //   return result;
  // }

  // template <typename J, typename... Args> J *CreateJob(Args &&...args) {
  //   J *result = nullptr;
  //   auto j = std::make_unique<J>(std::forward<Args>(args)...);
  //   result = j.get();
  //   jobs.Add(std::move(j));
  //   return result;
  // }

public:
  // void CancelJob();
  // void CancelJobs();
  // void PruneJob();
  void PrintJobs();
  int RunJobs();
  JobQueue &GetQueue() { return *jobQueue.get(); }

public:
  Driver &GetDriver() { return driver; }
  ToolChain &GetToolChain() { return tc; }
};
} // namespace stone

#endif
