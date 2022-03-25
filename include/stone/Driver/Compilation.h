#ifndef STONE_DRIVER_COMPILATION_H
#define STONE_DRIVER_COMPILATION_H

#include "stone/Core/LLVM.h"
#include "stone/Core/List.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/CompilationJob.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/ToolChain.h"

namespace stone {

class Job;
class TaskQueue;
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

  Driver &driver;
  ToolChain &tc;

  std::unique_ptr<CompilationStats> stats;
  std::unique_ptr<TaskQueue> tq;
  std::unique_ptr<llvm::opt::DerivedArgList> dal;

  // All the jobs the tool chain created --- lifetime management.
  llvm::SmallVector<std::unique_ptr<const Job>, 32> jobs;

public:
  Compilation(Driver &driver, ToolChain &tc,
              std::unique_ptr<llvm::opt::DerivedArgList> dal);

public:
  // template <typename T, typename... Args> T *CreateJob(Args &&...args) {
  //   auto result = new T(std::forward<Args>(args)...);
  //   jobs.emplace_back(result);
  //   return result;
  // }

public:
  // void CancelJob();
  // void CancelJobs();
  // void PruneJob();

  void PrintJobs();
  int RunJobs();

  TaskQueue &GetQueue() { return *tq.get(); }

public:
  Driver &GetDriver() { return driver; }
  ToolChain &GetToolChain() { return tc; }
};
} // namespace stone

#endif
