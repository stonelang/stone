#ifndef STONE_DRIVER_DRIVER_H
#define STONE_DRIVER_DRIVER_H

#include "stone/Basic/FileType.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Diag/DiagnosticEngine.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/CompilationEntity.h"
#include "stone/Driver/DriverInputFile.h"
#include "stone/Driver/DriverOptions.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/JobConstruction.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/ToolChain.h"
#include "stone/Stats/Stats.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/StringRef.h"

#include <functional>
#include <memory>
#include <string>

namespace llvm {
namespace opt {
class Arg;
class ArgList;
class OptTable;
class InputArgList;
class DerivedArgList;
} // namespace opt
} // namespace llvm

namespace stone {

class Driver;

class TopLevelCompilationEntities final {
  friend Driver;
  friend Compilation;

  // A graph of JobConstructions -- do not mark as cons since the
  // JobConstruction creates the Job
  llvm::SmallVector<const CompilationEntity *, 8> topLevelJobConstructions;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const CompilationEntity *, 8> topLevelJobs;

  // A graph of the top level jobs built by the driver
  llvm::SmallVector<const CompilationEntity *, 8> topLevelExternalJobs;

  llvm::SmallVector<const CompilationEntity *> entities;

public:
  void AddTopLevelJobConstruction(const CompilationEntity *entity) {
    topLevelJobConstructions.push_back(entity);
  }
  void AddTopLevelJob(const CompilationEntity *entity) {
    topLevelJobs.push_back(entity);
  }
  void AddTopLevelExternalJob(const CompilationEntity *entity) {
    topLevelExternalJobs.push_back(entity);
  }

public:
  bool HasTopLevelJobConstructions() {
    return (!topLevelJobConstructions.empty() &&
            topLevelJobConstructions.size() > 0);
  }
  bool HasTopLevelJobs() {
    return (!topLevelJobs.empty() && topLevelJobs.size() > 0);
  }
  bool HasTopLevelExternalJobs() {
    return (!topLevelExternalJobs.empty() && topLevelExternalJobs.size() > 0);
  }

public:
  /// Get each top level job
  void ForEachTopLevelJobConstruction(
      std::function<void(const CompilationEntity *entity)> callback);

  /// Get each top level job
  void ForEachTopLevelJob(
      std::function<void(const CompilationEntity *entity)> callback);

  /// Get each top level job
  void ForEachTopLevelExternalJob(
      std::function<void(const CompilationEntity *entity)> callback);
};

// Generally, we are compiling and linking -- they are special, so we treat them
// as such.
class ModuleEntities final : public DriverAllocation<ModuleEntities> {

  llvm::SmallVector<const CompilationEntity *, 8> entities;

public:
  explicit ModuleEntities() {}

public:
  void AddEntity(const CompilationEntity *entity) {}

public:
  static ModuleEntities *Create(const Driver &driver);
};

class LinkEntities final : public DriverAllocation<LinkEntities> {

  llvm::SmallVector<const CompilationEntity *, 8> entities;

public:
  explicit LinkEntities() {}

public:
  void AddEntity(const CompilationEntity *entity) {}
  bool HasEntities() { return !entities.empty() && entities.size() > 0; }

public:
  static LinkEntities *Create(const Driver &driver);
};

class BuildingJobConstructionEntities final
    : public DriverAllocation<BuildingJobConstructionEntities> {

  Driver &driver;
  ModuleEntities *moduleEntities = nullptr;
  LinkEntities *linkEntities = nullptr;
  GeneratePCHJobConstruction* pchJobConstruction = nullptr;
public:
  BuildingJobConstructionEntities(Driver &driver);
  void Initialize();

public:
  bool HasModuleEntities() { return moduleEntities != nullptr; }
  ModuleEntities *GetModuleEntities() { return moduleEntities; }

  bool HasLinkEntities() { return linkEntities != nullptr; }
  LinkEntities *GetLinkEntities() { return linkEntities; }

public:
  GeneratePCHJobConstruction* GetGeneratePCHJobConstruction();
  void CreateLinkJobConstruction();
  void CreateMergeModuleJobConstruction();
  void CreateAutolinkExtractJobConstruction();

public:
  /// < FileType handles
  Status HandleStoneFileType(const DriverInputFile *input);
  Status HandleObjectFileType(const DriverInputFile *input);
  Status HandleAutoLinkFileType(const DriverInputFile *input);
  Status HandleStoneModuleFileType(const DriverInputFile *input);

public:
  void FinishBuilding();

public:
  static BuildingJobConstructionEntities *Create(Driver &driver);
};

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

  /// The top-level compilation entities
  std::unique_ptr<TopLevelCompilationEntities> topLevelEntities;

  /// Builds the job entities
  // TopLevelJobEntitiesBuilder jobEntitiesBuilder;

  /// Build the JobConstruction entities
  // BuildingJobConstructionEntities jobConstructionEntitiesBuilder;

public:
  Driver();
  ~Driver();

public:
  DiagnosticEngine &GetDiags() { return diags; }
  llvm::opt::OptTable &GetOptTable() { return *optTable; }
  const llvm::opt::OptTable &GetOptTable() const { return *optTable; }

  llvm::opt::InputArgList &GetInputArgList() { return *inputArgList; }
  llvm::opt::DerivedArgList &GetDerivedArgList() { return *derivedArgList; }

  DriverOptions &GetDriverOptions() { return driverOpts; }
  const DriverOptions &GetDriverOptions() const { return driverOpts; }

  bool HasToolChain() const { return toolChain != nullptr; }
  ToolChain &GetToolChain() { return *toolChain; }
  const ToolChain &GetToolChain() const { return *toolChain; }

  bool HasTaskQueue() { return taskQueue != nullptr; }
  TaskQueue &GetTaskQueue() { return *taskQueue; }
  const TaskQueue &GetTaskQueue() const { return *taskQueue; }

  llvm::sys::TimePoint<> GetBuildStartTime() { return buildStartTime; }
  llvm::sys::TimePoint<> GetBuildLastTime() { return buildLastTime; }

  TopLevelCompilationEntities &GetTopLevelEntities() {
    return *topLevelEntities;
  }
  const TopLevelCompilationEntities &GetTopLevelEntities() const {
    return *topLevelEntities;
  }

public:
  /// Add the diagnostic consumer
  void AddDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.AddConsumer(consumer);
  }

  /// Remove the diagnostic consumer
  void RemoveDiagnosticConsumer(DiagnosticConsumer &consumer) {
    diags.RemoveConsumer(consumer);
  }
  /// Parse the arg strings only
  llvm::opt::InputArgList *ParseArgStrings(llvm::ArrayRef<const char *> args);

  /// Convert the inpurt args to driver options
  Status ConvertArgStrings(const llvm::opt::InputArgList &argList);

  /// Set the main exec path
  void SetMainExecutablePath(llvm::StringRef executablePath) {
    driverOpts.mainExecutablePath = executablePath;
  }

  /// Set the main exec path
  void SetMainExecutableName(llvm::StringRef executableName) {
    driverOpts.mainExecutableName = executableName;
  }
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

public:
  Status
  BuildTopLevelJobConstructionEntities(TopLevelCompilationEntities &entities,
                                       CompileInvocationMode cim);

  Status BuildMultipleCompileInvocation(
      TopLevelCompilationEntities &entities,
      BuildingJobConstructionEntities *buildingEntities);

  Status BuildSingleCompileInvocation(
      TopLevelCompilationEntities &entities,
      BuildingJobConstructionEntities *buildingEntities);

  Status BuildBatchCompileInvocation(
      TopLevelCompilationEntities &entities,
      BuildingJobConstructionEntities *buildingEntities);

  CompileJobConstruction *
  CreateCompileJobConstruction(const DriverInputFile *input = nullptr);

public:
  Status BuildTopLevelJobEntities(TopLevelCompilationEntities &entities);

public:
  /// Construct a compilation object for a given ToolChain
  ///
  /// If \p AllowErrors is set to \c true, this method tries to build a
  /// compilation even if there were errors.
  ///
  /// \return A Compilation, or null pointer if none was built for the given
  /// argument vector. A null return value does not necessarily indicate an
  /// error condition; the diagnostics should be queried to determine if an
  /// error occurred.
  Compilation *BuildCompilation(const ToolChain &toolChain);

public:
  /// Creates a DriverInput file using a
  DriverInputFile *CreateInput(llvm::StringRef fileName,
                               file::FileType = file::FileType::None);

  /// Build the job-constructions
  Status BuildJobConstructions();

  const JobConstruction *
  CastToJobConstruction(const CompilationEntity *entity) {
    return llvm::dyn_cast<JobConstruction>(entity);
  }

  /// Print the list of job constructions in a Compilation.
  void PrintJobConstructions() const;

public:
  using JobCacheMap =
      llvm::DenseMap<std::pair<const CompilationEntity *, const ToolChain *>,
                     Job *>;
  /// Build the jobs
  Status BuildTopLevelJobs();

  void ComputeJobMainOutput(const JobConstruction *jobConstruction);

  const Job *CastToJob(const CompilationEntity *entity) {
    return llvm::dyn_cast<Job>(entity);
  }
  /// Print the list of Actions in a Compilation.
  void PrintJobs() const;

public:
  /// These are use a lot -- put them here for covenience.
  CompileInvocationMode GetCompileInvocationMode() const {
    return GetDriverOptions().GetDriverOutputInfo().GetCompileInvocationMode();
  }
  bool IsMultipleCompileInvocation() const {
    return GetDriverOptions()
        .GetDriverOutputInfo()
        .IsMultipleCompileInvocation();
  }
  bool IsSingleCompileInvocation() const {
    return GetDriverOptions().GetDriverOutputInfo().IsSingleCompileInvocation();
  }
  bool IsBatchCompileInvocation() const {
    return GetDriverOptions().GetDriverOutputInfo().IsBatchCompileInvocation();
  }

  LinkMode GetLinkMode() const {
    return GetDriverOptions().GetDriverOutputInfo().GetLinkMode();
  }
  bool ShouldLink() const {
    return GetDriverOptions().GetDriverOutputInfo().ShouldLink();
  }

  bool IsStaticLibraryLink() const {
    return GetDriverOptions().GetDriverOutputInfo().IsStaticLibraryLink();
  }
  bool IsDynamicLibraryLink() const {
    return GetDriverOptions().GetDriverOutputInfo().IsDynamicLibraryLink();
  }
  bool IsExecutableLink() const {
    return GetDriverOptions().GetDriverOutputInfo().IsExecutableLink();
  }
  bool ShouldGenerateModule() const {
    return GetDriverOptions().GetDriverOutputInfo().ShouldGenerateModule();
  }

  file::FileType GetOutputFileType() const {
    return GetDriverOptions().GetDriverOutputInfo().GetOutputFileType();
  }

public:
  /// Print the help text.
  ///
  /// \param ShowHidden Show hidden options.
  void PrintHelp(bool showHidden = false) const;
  /// Print the driver version.
  void PrintVersion(const ToolChain &toolChain, llvm::raw_ostream &os) const;

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