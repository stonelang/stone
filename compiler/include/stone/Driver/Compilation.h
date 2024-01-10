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
  const Driver &driver;
  class Implementation;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> jobs;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> externalJobs;

public:
  Compilation(const Driver &driver);

public:
  Status Setup();
  const Driver &GetDriver() const { return driver; }

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

  void AddJob(const Job *job);
  void AddExternalJob(const Job *job);

public:
  bool HasJobs() const { return !jobs.empty() && jobs.size() > 0; }
  bool HasExternalJobs() const {
    return !externalJobs.empty() && externalJobs.size() > 0;
  }

public:
  static Compilation *Create(const Driver &driver);
};

} // namespace stone

#endif
