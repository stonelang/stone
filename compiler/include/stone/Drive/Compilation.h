#ifndef STONE_DRIVER_COMPILATION_H
#define STONE_DRIVER_COMPILATION_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/List.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Drive/CompilationJob.h"
#include "stone/Drive/DriverOptions.h"
#include "stone/Drive/TaskQueue.h"
#include "stone/Drive/ToolChain.h"

namespace stone {

class Job;
class TaskQueue;
class BuildSystem;
class Compilation;
class CompilationListener;

class CompilationStats final : public Stats {
  Compilation &compilation;

public:
  CompilationStats(Compilation &compilation)
      : Stats("Compilation statistics:"), compilation(compilation) {}
  void Print(ColorfulStream &stream) override;
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
  stone::Error RunJobs();

  TaskQueue &GetQueue() { return *tq.get(); }

public:
  Driver &GetDriver() { return driver; }
  ToolChain &GetToolChain() { return tc; }
};
} // namespace stone

#endif
