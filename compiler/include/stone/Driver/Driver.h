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
class CompilationEntities;

// class FileTypeExecution {
//   Driver &driver;

// public:
//   FileTypeExecution(Driver &driver) : driver(driver) {}

//   // virtual void CompletedFileType();
// public:
//   // void Execute();
// };

// class StoneFileTypeExecution final : public FileTypeExecution {
//   Driver &driver;

// public:
//   StoneFileTypeExecution(Driver &driver) : FileTypeExecution(driver) {}

// public:
//   // void Execute();

//   // FileType GetSelfFileType();
// };

class CompileStyleConsumer {};

class CompileStyle {
  llvm::SmallVector<const CompileStyleConsumer *> consumers;
};
// class CompileStyle {

// protected:
//   Driver &driver;

//   // MergeModuleJobConstruction *mergeModuleJobConstruction = nullptr;
//   // LinkJobConstruction *linkJobConstruction = nullptr;

// public:
//   CompileStyle(Driver &driver);

//   //~CompileStyle();

// public:
// virtual Status BuildCompilationEntities(CompilationEntities &entities);
//  virtual Status HandleStoneFileType();
//  virtual Status HandleObjectFileType();
//  virtual Status HandleAutoLinkFileType();

// std::unique_ptr<FileTypeExecution> CreateTypeExecution(FileType fileType);
// CompilationEntity *CreateCompilationEntity(const DriverInputFile
// *inputFile);

// void SetLinkJobConstruction(LinkJobConstruction *jobConstruction);
// void
// SetMergeModuleJobConstruction(MergeModuleJobConstruction *jobConstruction);

// public:
//   bool HasGeneratePCHJobConstruction() const;
//   bool HasLinkJobConstruction() const;
// };

// class NormalCompileStyle final : public CompileStyle {

//   // GeneratePCHJobAction *generatePCHJobConstruction = nullptr;

// public:
//   NormalCompileStyle(Driver &driver);

// public:
//   // Status BuildCompilationEntities(CompilationEntities &entities) override;
//   // void SetGeneratePCHJobAction(GeneratePCHJobAction *jobConstruction);
// };

// class SingleCompileStyle final : public CompileStyle {
// public:
//   SingleCompileStyle(Driver &driver);

// public:
//   // Status BuildCompilationEntities(CompilationEntities &entities) override;
// };

// class BuildingJobConstructionEntities final {

//   MergeModuleJobConstruction *mergeModuleJobConstruction = nullptr;
//   LinkJobConstruction *linkJobConstruction = nullptr;

// public:
//   BuildingJobConstructionEntities(Driver &driver);
//   ~BuildingJobConstructionEntities();

// public:
//   void BuildForNormalCompileStyle();
//   void BuildForSingleCompileStyle();
//   void BuildForFlatCompileStyle();
// };

// class BuildingJobEntities final {
// public:
//   BuildingJobEntities(Driver &driver);
//   ~BuildingJobEntities();

// public:
// };

// class BuildingCompilationEntities final {
// public:
//   BuildingJobEntities jobEntities;
//   BuildingJobConstructionEntities jobConstructionEntities;
// };

class Driver;
class BuildingCompilationEntities;

class TopLevelCompilationEntitiesConsumer
    : public DriverAllocation<TopLevelCompilationEntitiesConsumer> {

protected:
  llvm::SmallVector<const CompilationEntity *> entities;

protected:
  void AddCompilationEntity(const CompilationEntity *entity) {
    entities.push_back(entity);
  }

public:
  TopLevelCompilationEntitiesConsumer();

public:
  virtual void CompletedCompilationEntity(const CompilationEntity *entity);
  virtual void Finish();
};

class LinkJobConstructionEntitiesConsumer final
    : public TopLevelCompilationEntitiesConsumer {
public:
  LinkJobConstructionEntitiesConsumer();

public:
  void CompletedCompilationEntity(const CompilationEntity *entity) override;
  void Finish() override;

public:
  static LinkJobConstructionEntitiesConsumer *Create(Driver &driver);
};

class MergeModuleJobConstructionEntitiesConsumer final
    : public TopLevelCompilationEntitiesConsumer {

public:
  MergeModuleJobConstructionEntitiesConsumer();

public:
  void CompletedCompilationEntity(const CompilationEntity *entity) override;
  void Finish() override;

public:
  static MergeModuleJobConstructionEntitiesConsumer *Create(Driver &driver);
};

class BuildingJobConstructionEntities final {

  Driver &driver;
  llvm::SmallVector<TopLevelCompilationEntitiesConsumer *> consumers;

public:
  BuildingJobConstructionEntities(Driver &driver);

public:
  Status BuildForCompileStyle(CompileStyleKind kind);
  Status BuildForNormalCompileStyle();
  Status BuildForSingleCompileStyle();
  Status BuildForFlatCompileStyle();

public:
  bool IsTopLvelJobConstruction() { return consumers.size() > 0; }
  void ForEachConsumer(
      std::function<void(TopLevelCompilationEntitiesConsumer *consumer)> fn);
  void CreateCompileJobConstruction(const DriverInputFile *input);

  void CompletedCompilationEntity(const CompilationEntity *entity);
  // void CompletedCompilationEntity(const DriverInputFile *entity);

public:
  void AddConsumer(TopLevelCompilationEntitiesConsumer *consumer);
};

class BuildingJobEntities final {
  Driver &driver;

public:
  BuildingJobEntities(Driver &driver);
  //~BuildingJobEntities();
};

class BuildingCompilationEntities final {
  Driver &driver;

public:
  BuildingJobEntities jobEntities;
  BuildingJobConstructionEntities jobConstructionEntities;

public:
  BuildingCompilationEntities(Driver &driver);

  //~BuildingCompilationEntities();

public:
  Status BuildCompilationEntities(CompilationEntities &entities);

  BuildingJobEntities &GetBuildingJobEntities() { return jobEntities; }
  BuildingJobConstructionEntities &GetBuildingJobConstructionEntities() {
    return jobConstructionEntities;
  }

  void Finish();
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

  // std::unique_ptr<CompileStyle> compileStyle;

  std::unique_ptr<CompilationEntities> compilationEntities;

  // BuildingJobEntities jobEntities;
  // BuildingJobConstructionEntities jobConstructionEntities;

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

  bool HasToolChain() { return toolChain != nullptr; }
  ToolChain &GetToolChain() { return *toolChain; }
  const ToolChain &GetToolChain() const { return *toolChain; }

  bool HasTaskQueue() { return taskQueue != nullptr; }
  TaskQueue &GetTaskQueue() { return *taskQueue; }
  const TaskQueue &GetTaskQueue() const { return *taskQueue; }

  llvm::sys::TimePoint<> GetBuildStartTime() { return buildStartTime; }
  llvm::sys::TimePoint<> GetBuildLastTime() { return buildLastTime; }

  bool HasCompilationEntities() { return compilationEntities != nullptr; }
  CompilationEntities &GetCompilationEntities() { return *compilationEntities; }
  const CompilationEntities &GetCompilationEntities() const {
    return *compilationEntities;
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

  // std::unique_ptr<CompileStyle> CreateCompileStyle();

  Status BuildCompilationEntities(CompilationEntities &entities);

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

  void ComputeMainOutputForTopLevelJob(const JobConstruction *jobConstruction);

  const Job *CastToJob(const CompilationEntity *entity) {
    return llvm::dyn_cast<Job>(entity);
  }
  /// Print the list of Actions in a Compilation.
  void PrintJobs() const;

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