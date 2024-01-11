#ifndef STONE_DRIVER_DRIVER_COMPILATION_H
#define STONE_DRIVER_DRIVER_COMPILATION_H

#include "stone/Driver/Driver.h"
#include "stone/Driver/Job.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Chrono.h"

#include <memory>
#include <vector>

namespace llvm {
namespace opt {
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {

using CommandSet = llvm::SmallPtrSet<const Job *, 16>;

using CommandSetVector = llvm::SetVector<const Job *>;

using BatchPartition = std::vector<std::vector<const Job *>>;

class CompilationResult final {
public:
  /// Set to true if any job exits abnormally (i.e. crashes).
  bool hadAbnormalExit;
  /// The exit code of this driver process.
  int exitCode;

public:
  CompilationResult(const CompilationResult &) = delete;
  CompilationResult &operator=(const CompilationResult &) = delete;

  CompilationResult(CompilationResult &&) = default;
  CompilationResult &operator=(CompilationResult &&) = default;

public:
  CompilationResult();
};

class Compilation final : public DriverAllocation<Compilation> {
  Driver &driver;
  class Implementation;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> jobs;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> externalJobs;

public:
  Compilation(Driver &driver);

public:
  Status Setup();
  Driver &GetDriver() { return driver; }

  /// TODO:
  bool ShouldRunSingleJob() const { return false; }
  bool HasParallelTasks() { return false; }
  bool ShouldSupportParallelExecution() const { return true; }

public:
  /// Print the list of Actions in a Compilation.
  void PrintJobs(llvm::raw_ostream &os) const;

public:
  CompilationResult RunJobs();

private:
  friend Driver;
  friend Implementation;

  void AddTopLevelJob(const Job *job);
  void AddTopLevelExternalJob(const Job *job);

public:
  bool HasTopLevelJobs() const { return !jobs.empty() && jobs.size() > 0; }

  /// Get each top level job
  void ForEachTopLevelJob(std::function<void(const Job *job)> callback) {
    for (auto jb : jobs) {
      callback(jb);
    }
  }
  /// Get each top level job
  void
  ForEachTopLevelExternalJob(std::function<void(const Job *job)> callback) {
    for (auto jb : externalJobs) {
      callback(jb);
    }
  }
  bool HasTopLevelExternalJobs() const {
    return !externalJobs.empty() && externalJobs.size() > 0;
  }

public:
  static Compilation *Create(Driver &driver);
};

} // namespace stone

#endif
