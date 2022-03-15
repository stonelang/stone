#ifndef STONE_COMPILATION_COMPILATION_H
#define STONE_COMPILATION_COMPILATION_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Intent.h"
#include "stone/Driver/JobQueue.h"
#include "stone/Driver/ToolChain.h"

namespace stone {

class Job;
class JobQueue;
class Driver;
class Compilation;

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

  /// This is mostly only here for lifetime management.
  llvm::SmallVector<std::unique_ptr<const Intent>, 32> intents;

  /// The Jobs which will be performed by this compilation.
  llvm::SmallVector<std::unique_ptr<const Job>, 32> jobs;

public:
  Compilation(Driver &driver);

public:
  /// TODO: cleanup
  // template <typename I, typename... Args> I *CreateIntent(Args &&...args) {
  //   I *result = nullptr;
  //   auto i = std::make_unique<I>(std::forward<Args>(args)...);
  //   result = i.get();
  //   intents.Add(std::move(i));
  //   return result;
  // }

  template <typename I, typename... Args> I *CreateIntent(Args &&...args) {
    auto result = new I(std::forward<Args>(args)...);
    intents.emplace_back(result);
    return result;
  }

  // template <typename J, typename... Args> J *CreateJob(Args &&...args) {
  //   J *result = nullptr;
  //   auto j = std::make_unique<J>(std::forward<Args>(args)...);
  //   result = j.get();
  //   jobs.Add(std::move(j));
  //   return result;
  // }

public:
  void Run();
  // void CancelJob();
  // void CancelJobs();
  // void PruneJob();
  void PrintJobs();
  void RunJobs();
  JobQueue &GetQueue() { return *jobQueue.get(); }

public:
  Driver &GetDriver() { return driver; }

} // namespace stone

#endif
