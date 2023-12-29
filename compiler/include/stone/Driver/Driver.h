#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/Status.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DriverInvocation.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Stats/Stats.h"

namespace stone {

class Driver final {

  DriverInvocation &invocation;
  std::unique_ptr<ToolChain> toolChain;
  std::unique_ptr<Compilation> compilation;
  std::unique_ptr<stone::TaskQueue> taskQueue;

  // A graph of JobConstructions.
  llvm::SmallVector<const JobConstruction *, 8> topLevelJobConstructions;

  /// The allocator used to create Driver objects.
  /// Driver objects are never destructed; rather, all memory associated
  /// with the Driver objects will be released when the Driver
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

  /// Stats collections
  // std::unique_ptr<DriverStatsReporter> statsReporter;

public:
  Driver(DriverInvocation &invocation);
  ~Driver();

  Status Setup();

public:
  /// Allocate - Allocate memory from the Driver bump pointer.
  void *Allocate(unsigned long bytes, unsigned alignment = 8) const {
    if (bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(bytes, alignment);
  }

public:
  bool HasToolChain() { return toolChain != nullptr; }
  ToolChain &GetToolChain() { return *toolChain; }
  const ToolChain &GetToolChain() const { return *toolChain; }

  bool HasTaskQueue() { return taskQueue != nullptr; }
  TaskQueue &GetTaskQueue() { return *taskQueue; }
  const TaskQueue &GetTaskQueue() const { return *taskQueue; }

  bool HasCompilation() { return compilation != nullptr; }
  Compilation &GetCompilation() { return *compilation; }

  const DriverInvocation &GetInvocation() const { return invocation; }

public:
  DiagnosticEngine &GetDiags() { return invocation.GetDiags(); }

  // DriverStatsReporter &GetStatsReporter() { return *statsReporter; }

public:
  /// Creates an appropriate ToolChain for a given driver, given the target
  /// specified in \p Args (or the default target). Sets the value of \c
  /// DefaultTargetTriple from \p Args as a side effect.
  ///
  /// \return A ToolChain, or nullptr if an unsupported target was specified
  /// (in which case a diagnostic error is also signalled).
  ///
  /// This uses a std::unique_ptr instead of returning a toolchain by value
  /// because ToolChain has virtual methods.
  std::unique_ptr<ToolChain> BuildToolChain(ToolChainKind kind);

  class BuildingCompilationRAII final {
    Driver &driver;

  public:
    llvm::SmallVector<JobConstructionInput, 2> moduleInputs;
    llvm::SmallVector<JobConstructionInput, 2> linkerInputs;
    llvm::SmallVector<const JobConstruction *, 8> topLevelJobConstructions;

  public:
    BuildingCompilationRAII(Driver &driver) : driver(driver) {
      moduleInputs.clear();
      linkerInputs.clear();
    }
    ~BuildingCompilationRAII();

  public:
    void AddModuleInput(const JobConstructionInput input) {
      moduleInputs.push_back(input);
    }

    void AddLinkerInput(const JobConstructionInput input) {
      linkerInputs.push_back(input);
    }

    bool HasLinkerInputs() { return !linkerInputs.empty(); }

    void AddTopLevelJobConstruction(const JobConstruction *construction) {
      topLevelJobConstructions.push_back(construction);
    }
  };
  /// Construct a compilation object for a given ToolChain and command line
  /// argument vector.
  ///
  /// If \p AllowErrors is set to \c true, this method tries to build a
  /// compilation even if there were errors.
  ///
  /// \return A Compilation, or nullptr if none was built for the given argument
  /// vector. A null return value does not necessarily indicate an error
  /// condition; the diagnostics should be queried to determine if an error
  /// occurred.
  std::unique_ptr<Compilation> BuildCompilation(CompilationKind kind);

  /// Build a quadratic compilation
  std::unique_ptr<Compilation>
  BuildNormalCompilation(BuildingCompilationRAII &buildingCompilation);

  /// Build a flat compilation
  std::unique_ptr<Compilation>
  BuildFlatCompilation(BuildingCompilationRAII &buildingCompilation);

  /// Build a single compilation
  std::unique_ptr<Compilation>
  BuildSingleCompilation(BuildingCompilationRAII &buildingCompilation);

  ///
  std::unique_ptr<Compilation>
  BuildCPUCountCompilation(BuildingCompilationRAII &buildingCompilation);

public:
  /// Build the job-constructions
  Status BuildTopLevelJobConstructions();

  /// Add a ob-constructions
  Status BuildTopLevelJobConstruction();

  // /// Create the job-constructions
  // JobConstruction *CreateJobConstruction();

  /// Add a ob-constructions
  JobConstruction *CreateCompileJobConstruction(Driver &driver);

  // /// Add a ob-constructions
  // JobConstruction *CreateLinkJobConstruction(Driver &driver);

  void ForEachJobConstruction(
      std::function<void(JobConstruction &construction)> callback);

  /// Build the jobs
  Status BuildJobs();

  /// Print the list of Actions in a Compilation.
  void PrintJobConstructions(const Compilation &compilation) const;

  /// Print the list of Actions in a Compilation.
  void PrintJobs(const Compilation &compilation) const;

public:
  /// Compute the task queue for this compilation and command line argument
  /// vector.
  ///
  /// \return A TaskQueue, or nullptr if an invalid number of parallel jobs is
  /// specified.  This condition is signalled by a diagnostic.
  std::unique_ptr<stone::TaskQueue>
  BuildTaskQueue(const Compilation &compilation);

public:
  /// Print the driver version.
  void PrintVersion(const ToolChain &toolChain, llvm::raw_ostream &os) const;
  /// Print the help text.
  ///
  /// \param ShowHidden Show hidden options.
  void PrintHelp(bool showHidden) const;

public:
  Status ExecuteCompilation();
};

} // namespace stone
#endif