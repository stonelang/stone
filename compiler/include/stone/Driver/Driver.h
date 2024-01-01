#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Option/Options.h"
#include "stone/Stats/Stats.h"

using namespace llvm::opt;

#include <memory>
#include <vector>

namespace llvm {
namespace opt {
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {

class Driver final {

  SrcMgr srcMgr;
  DiagnosticEngine diags{srcMgr};

  /// The options for compilation
  DriverOptions driverOpts;

  /// The driver options
  std::unique_ptr<llvm::opt::OptTable> optTable;

  /// The parsed input arg list
  std::unique_ptr<llvm::opt::InputArgList> inputArgList;

  /// The derived arg list
  std::unique_ptr<llvm::opt::DerivedArgList> derivedArgList;

  /// The tool chain to use to build the tools
  std::unique_ptr<ToolChain> toolChain;

  /// The task queue to run the jobs
  std::unique_ptr<stone::TaskQueue> taskQueue;

  /// The allocator used to create Driver objects.
  /// Driver objects are never destructed; rather, all memory associated
  /// with the Driver objects will be released when the Driver
  /// itself is destroyed.
  mutable llvm::BumpPtrAllocator allocator;

  /// Stats collections
  std::unique_ptr<DriverStatsReporter> stats;

  /// When the build was started.
  ///
  /// This should be as close as possible to when the driver was invoked, since
  /// it's used as a lower bound.
  llvm::sys::TimePoint<> buildStartTime;

  /// The time of the last compilation.
  ///
  /// If unknown, this will be some time in the past.
  llvm::sys::TimePoint<> buildLastTime = llvm::sys::TimePoint<>::min();

  std::unique_ptr<CompilationEntities> compilationEntities;

public:
  // A graph of JobConstructions.
  llvm::SmallVector<const JobConstruction *, 8> topLevelJobConstructions;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const Job *, 8> topLevelJobs;

public:
  Driver();
  ~Driver();
  Status Setup(const llvm::opt::InputArgList &argList);

public:
  /// Parse the arg strings only
  llvm::opt::InputArgList *ParseArgStrings(llvm::ArrayRef<const char *> args);

  /// Convert the inpurt args to driver options
  Status ConvertArgStrings(const llvm::opt::InputArgList &argList);

private:
  llvm::StringRef
  ComputeWorkingDirectory(const llvm::opt::InputArgList &argList);
  llvm::opt::DerivedArgList *
  TranslateInputArgList(const llvm::opt::InputArgList &argList,
                        llvm::StringRef workingDirectory);

  Status ComputeAction(const llvm::opt::DerivedArgList &argList);

public:
  void SetMainExecutablePath(llvm::StringRef executablePath) {
    driverOpts.mainExecutablePath = executablePath;
  }
  void SetMainExecutableName(llvm::StringRef executableName) {
    driverOpts.mainExecutablePath = executableName;
  }

  llvm::opt::OptTable &GetOptTable() { return *optTable; }
  const llvm::opt::OptTable &GetOptTable() const { return *optTable; }

  llvm::opt::InputArgList &GetInputArgList() { return *inputArgList; }
  llvm::opt::DerivedArgList &GetDerivedArgList() { return *derivedArgList; }

  bool HasToolChain() { return toolChain != nullptr; }
  ToolChain &GetToolChain() { return *toolChain; }
  const ToolChain &GetToolChain() const { return *toolChain; }

  bool HasTaskQueue() { return taskQueue != nullptr; }
  TaskQueue &GetTaskQueue() { return *taskQueue; }
  const TaskQueue &GetTaskQueue() const { return *taskQueue; }

  llvm::sys::TimePoint<> GetBuildStartTime() { return buildStartTime; }
  llvm::sys::TimePoint<> GetBuildLastTime() { return buildLastTime; }

  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }

  bool HasCompilationEntities() { return compilationEntities != nullptr; }
  CompilationEntities &GetCompilationEntities() { return *compilationEntities; }
  const CompilationEntities &GetCompilationEntities() const {
    return *compilationEntities;
  }

public:
  DiagnosticEngine &GetDiags() { return diags; }
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }

  bool HasStats() const { return stats != nullptr; }
  DriverStatsReporter &GetStats() { return *stats; }

public:
  /// Build the input files
  Status BuildInputFiles(const DerivedArgList &args, InputFileList &inputFiles);

  /// Computes the kind of tool-chain that is being built
  ToolChainKind ComputeToolChainKind(const llvm::opt::InputArgList &argList);
  /// Creates an appropriate ToolChain for a given driver, given the target
  /// specified in \p Args (or the default target). Sets the value of \c
  /// DefaultTargetTriple from \p Args as a side effect.
  ///
  /// \return A ToolChain, or nullptr if an unsupported target was specified
  /// (in which case a diagnostic error is also signalled).
  ///
  /// This uses a std::unique_ptr instead of returning a toolchain by value
  /// because ToolChain has virtual methods.
  ToolChain *BuildToolChain(ToolChainKind toolChainKind);

  /// Compute the compile invocation mode;
  CompileInvocationMode
  ComputeCompileInvocationMode(const DerivedArgList &args);

  /// Compute the compile invocation mode;
  Status BuildOutputs(const DerivedArgList &args);

  /// Compute the task queue for this compilation and command line argument
  /// vector.
  ///
  /// \return A TaskQueue, or nullptr if an invalid number of parallel jobs is
  /// specified.  This condition is signalled by a diagnostic.
  std::unique_ptr<stone::TaskQueue> BuildTaskQueue();

public:
  class BuildingJobConstructionsImpl;
  /// Build the job-constructions
  Status BuildJobConstructions();

  JobConstruction *CastToJobConstruction(JobConstructionInput input) {
    return input.get<JobConstruction *>();
  }
  DriverInputFile *CastToDriverInputFile(JobConstructionInput input) {
    return input.dyn_cast<DriverInputFile *>();
  }
  JobConstructionInput
  CastToJobConstructionInput(const DriverInputFile &input) {
    return const_cast<DriverInputFile *>(&input);
  }

  /// Print the list of Actions in a Compilation.
  void PrintJobConstructions() const;

public:
  class BuildingJobs;
  /// A map for caching Jobs for a given Action/ToolChain pair
  using JobCacheMap =
      llvm::DenseMap<std::pair<const JobConstruction *, const ToolChain *>,
                     Job *>;
  /// Build the jobs
  Status BuildTopLevelJobs();
  void ComputeMainOutputForTopLevelJob(JobConstruction *jobConstruction);

  /// Print the list of Actions in a Compilation.
  void PrintJobs() const;

public:
  /// < Driver support.

  /// Print the driver version.
  void PrintVersion(const ToolChain &toolChain, llvm::raw_ostream &os) const;
  /// Print the help text.
  ///
  /// \param ShowHidden Show hidden options.
  void PrintHelp(bool showHidden = false) const;

  /// Print Help, HelpHidden, or Version.
  ///
  /// \param ShowHidden Show hidden options.
  void PrintSupport() const;

public:
  /// Might this sort of compile have explicit primary inputs?
  /// When running a single compile for the whole module (in other words
  /// "whole-module-optimization" mode) there must be no -primary-input's and
  /// nothing in a (preferably non-existent) -primary-filelist. Left to its own
  /// devices, the driver would forget to omit the primary input files, so
  /// return a flag here.
  Status MightHaveExplicitPrimaryInputs(const CommandOutput &jobOutput) const;

public:
  /// Allocate - Allocate memory from the Driver bump pointer.
  void *Allocate(unsigned long bytes, unsigned alignment = 8) const {
    if (bytes == 0) {
      return nullptr;
    }
    return allocator.Allocate(bytes, alignment);
  }
};

} // namespace stone
#endif