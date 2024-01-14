#include "stone/Basic/Defer.h"
#include "stone/Basic/FileType.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/Job.h"
#include "stone/Strings.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;
using namespace llvm::opt;

Driver::Driver() : optTable(stone::CreateOptTable()) {}

Driver::~Driver() {}

llvm::opt::InputArgList *
Driver::ParseArgStrings(llvm::ArrayRef<const char *> args) {

  unsigned includedFlagsBitmask = 0;
  unsigned excludedFlagsBitmask = opts::NoDriverOption;
  unsigned missingArgIndex;
  unsigned missingArgCount;

  inputArgList = std::make_unique<InputArgList>(
      GetOptTable().ParseArgs(args, missingArgIndex, missingArgCount,
                              includedFlagsBitmask, excludedFlagsBitmask));

  assert(inputArgList && "No input argument list.");
  if (missingArgCount) {
    diags.PrintD(SrcLoc(), diag::err_missing_arg_value,
                 diag::LLVMStr(inputArgList->getArgString(missingArgIndex)),
                 diag::UInt(missingArgCount));
    return nullptr;
  }
  // Check for unknown arguments.
  for (const llvm::opt::Arg *arg : inputArgList->filtered(opts::UNKNOWN)) {
    diags.PrintD(SrcLoc(), diag::err_unknown_arg,
                 diag::LLVMStr(arg->getAsString(*inputArgList)));
    return nullptr;
  }
  return inputArgList.get();
}

Status Driver::ConvertArgStrings(const llvm::opt::InputArgList &args) {
  return DriverOptionsConverter(args, driverOpts, *this).Convert();
}

ToolChain *Driver::BuildToolChain(ToolChainKind toolChainKind) {
  switch (toolChainKind) {
  case ToolChainKind::Darwin:
    toolChain = std::make_unique<DarwinToolChain>(*this);
    break;
  case ToolChainKind::Linux:
    toolChain = std::make_unique<LinuxToolChain>(*this);
    break;
  case ToolChainKind::Windows:
    toolChain = std::make_unique<WindowsToolChain>(*this);
    break;
  default:
    llvm_unreachable("Unsupported OS -- cannot proceed with compilation!");
  }
  if (!toolChain) {
    return nullptr;
  }
  return toolChain.get();
}

void TopLevelCompilationEntities::ForEachTopLevelJobConstruction(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelJobConstruction : topLevelJobConstructions) {
    callback(topLevelJobConstruction);
  }
}

/// Get each top level job
void TopLevelCompilationEntities::ForEachTopLevelJob(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelJob : topLevelJobs) {
    callback(topLevelJob);
  }
}

/// Get each top level job
void TopLevelCompilationEntities::ForEachTopLevelExternalJob(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelExternalJob : topLevelExternalJobs) {
    callback(topLevelExternalJob);
  }
}

void BuildingCompilationEntitiesConsumer::ForEachCompilationEntity(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto entity : entities) {
    callback(entity);
  }
}

BuildingCompilationEntitiesConsumer::BuildingCompilationEntitiesConsumer(
    Driver &driver)
    : driver(driver) {}

void BuildingCompilationEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  llvm_unreachable("Only sub-classes can make this call");
}

void BuildingCompilationEntitiesConsumer::Finish() {
  llvm_unreachable("Only sub-classes can make this call");
}

BuildingJobConstructionEntitiesConsumer::
    BuildingJobConstructionEntitiesConsumer(Driver &driver)
    : BuildingCompilationEntitiesConsumer(driver) {

  // assert(driver.GetDriverOptions().GetDriverOutputInfo().ShouldLink());
}

BuildingJobConstructionEntitiesConsumer *
BuildingJobConstructionEntitiesConsumer::Create(Driver &driver) {
  return new (driver) BuildingJobConstructionEntitiesConsumer(driver);
}

void BuildingJobConstructionEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {

  // if (llmv::isa<CompileJobConstruction>(entity)) {

  // }
  /// Note, this may not be a top level -- could just be an object file
  // asert(entity->IsTopLevel());
  /// TODO: Some checks
  AddCompilationEntity(entity);

  // why not just construct the job here
}

void BuildingJobConstructionEntitiesConsumer::Finish() {

  // if (HasTopLevelCompilationEntities()) {
  //   auto const outputInfo = driver.GetDriverOptions().GetDriverOutputInfo();
  //   if (outputInfo.IsStaticLibraryLink()) {
  //     auto topLevelJC = StaticLinkJobConstruction::Create(
  //         driver, entities, outputInfo.GetLinkMode());
  //     // topLevelJC->AddIsTopLevel();

  //     driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
  //         StaticLinkJobConstruction::Create(driver, entities,
  //                                           outputInfo.GetLinkMode()));
  //   } else {
  //     driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
  //         DynamicLinkJobConstruction::Create(
  //             driver, entities, outputInfo.GetLinkMode(),
  //             outputInfo.HasLTO()));
  //   }
  // }

  ForEachCompilationEntity([&](const CompilationEntity *entity) {
    if (driver.IsMultipleCompileInvocation()) {

      if (llmv::isa<CompileJobConstruction>(entity)) {
      }
    }
  });
}

void BuildingJobConstructionEntitiesConsumer::
    CreateGeneratePCHJobConstruction() {}

void BuildingJobConstructionEntitiesConsumer::CreateLinkJobConstruction() {}

void BuildingJobConstructionEntitiesConsumer::
    CreateMergeModuleJobConstruction() {}

void BuildingJobConstructionEntitiesConsumer::
    CreateAutolinkExtractJobConstruction() {}

// MergeModuleJobConstructionEntitiesConsumer::
//     MergeModuleJobConstructionEntitiesConsumer(Driver &driver)
//     : BuildingCompilationEntitiesConsumer(driver) {

//   assert(!driver.IsSingleCompileInvocation());
//   assert(driver.ShouldGenerateModule());
// }

// MergeModuleJobConstructionEntitiesConsumer *
// MergeModuleJobConstructionEntitiesConsumer::Create(Driver &driver) {

//   if (driver.IsSingleCompileInvocation()) {
//     return nullptr;
//   }

//   if (!driver.ShouldGenerateModule()) {
//     return nullptr;
//   }

//   return new (driver) MergeModuleJobConstructionEntitiesConsumer(driver);
// }

// void MergeModuleJobConstructionEntitiesConsumer::CompletedCompilationEntity(
//     const CompilationEntity *entity) {
//   // Add to the special Module

//   if (auto incrementalJobEntity =
//           llvm::dyn_cast<IncrementalJobConstruction>(entity)) {
//     // Take the upper bound of the status of any incremental inputs to
//     // ensure that the merge-modules job gets run if *any* input job is run.
//     // const auto conservativeStatus =
//     //     std::max(StatusBound.status, IJA->getInputInfo().status);
//     // // The modification time here is not important to the rest of the
//     // // incremental build. We take the upper bound in case an attempt to
//     // // compare the swiftmodule output's mod time and any input files is
//     // // made. If the compilation has been correctly scheduled, the
//     // // swiftmodule's mod time will always strictly exceed the mod time of
//     // // any of its inputs when we are able to skip it.
//     // const auto conservativeModTime = std::max(
//     //     StatusBound.previousModTime, IJA->getInputInfo().previousModTime);
//     // StatusBound = InputInfo{conservativeStatus, conservativeModTime};
//   }
// }

// void MergeModuleJobConstructionEntitiesConsumer::Finish() {

//   // Ok, now we can try to create the link job
//   // But, you have to create a special M
// }

// BuildingJobConstructionEntities::BuildingJobConstructionEntities(Driver
// &driver)
//     : driver(driver) {}

// void BuildingJobConstructionEntities::AddConsumer(
//     BuildingCompilationEntitiesConsumer *consumer) {
//   if (consumer) {
//     consumers.push_back(consumer);
//   }
// }

// Status BuildingJobConstructionEntities::BuildForCompileInvocation(
//     CompileInvocationMode compileStyle) {
//   switch (compileStyle) {
//   case CompileInvocationMode::Multiple:
//     return BuildForMultipleCompileInvocation();
//   case CompileInvocationMode::Single:
//     return BuildForSingleCompileInvocation();
//   case CompileInvocationMode::Batch:
//     return BuildForBatchCompileInvocation();
//   default:
//     llvm_unreachable("Invalid compile invocation kind");
//   }
// }

// CompileJobConstruction *
// BuildingJobConstructionEntities::CreateCompileJobConstruction(
//     const DriverInputFile *input) {

// // if (args.hasArg(opts::::EmbedBitCode)) {
// // }
// /// Check that it requires a PCH
// // CompileJobConstruction *compileJobConstruction = nullptr;
// if (input) {
//   return CompileJobConstruction::Create(
//       driver, input,
//       driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
// }

// return CompileJobConstruction::Create(
//     driver,
//     driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());

// //   if (driver.IsSingleCompileInvocation()) {
// //   }
// // }

// if (IsTopLevelJobConstruction()) {
//   CompletedCompilationEntity(compileJobConstruction);

// } else {
//   driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
//       compileJobConstruction);
//   return compileJobConstruction;
// }
//}

// void BuildingJobConstructionEntities::CompletedCompilationEntity(
//     const CompilationEntity *entity) {

//   ForEachConsumer([&](BuildingCompilationEntitiesConsumer *consumer) {
//     consumer->CompletedCompilationEntity(entity);
//   });
// }

// Status BuildingJobConstructionEntities::BuildForMultipleCompileInvocation() {
//   assert(driver.IsMultipleCompileInvocation());

// driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//     [&](const DriverInputFile *input) {
//       switch (input->GetFileType()) {
//       case FileType::Stone: {
//         assert(input->IsPartOfStoneCompilation());
//         CompletedCompilationEntity(CreateCompileJobConstruction(input));
//         break;
//       }
//       case FileType::Object: {
//         CompletedCompilationEntity(input);
//         break;
//       }
//       default:
//         llvm_unreachable(" Invalid file type");
//       }
//     });

//   return Status();
// }
// Status BuildingJobConstructionEntities::BuildForSingleCompileInvocation() {
//   assert(driver.IsSingleCompileInvocation());

// auto compileJobConstruction = CreateCompileJobConstruction();
// assert(compileJobConstruction);

// driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//     [&](const DriverInputFile *input) {
//       assert(input->IsPartOfStoneCompilation());
//       auto currentInput = driver.CastToJobConstruction(input);
//       compileJobConstruction->AddInput(currentInput);
//     });
// CompletedCompilationEntity(compileJobConstruction);

// return Status();
//}
// Status BuildingJobConstructionEntities::BuildForBatchCompileInvocation() {
//   assert(driver.IsBatchCompileInvocation());

//   return Status();
// }

// void BuildingJobConstructionEntities::ForEachConsumer(
//     std::function<void(BuildingCompilationEntitiesConsumer *consumer)> fn) {
//   for (auto consumer : consumers) {
//     fn(consumer);
//   }
// }

// void BuildingJobConstructionEntities::Finish() {

//   ///
// }

// TopLevelJobEntitiesBuilder::TopLevelJobEntitiesBuilder(Driver &driver)
//     : driver(driver) {}

// Status TopLevelJobEntitiesBuilder::BuildTopLevelJobEntities(
//     TopLevelCompilationEntities &entities) {

//   entities.ForEachTopLevelJobConstruction([&](const CompilationEntity
//   *entity) {
//     if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
//       entities.AddTopLevelJob(BuildTopLevelJob(jc));
//     }
//     // auto jb =
//     //
//     jobEntitiesBuilder.BuildTopLevelJob(llvm::dyn_cast<JobConstruction>(entity));

//     // auto jc = llvm::dyn_cast<JobConstruction>(entity);
//     // auto jb = const_cast<JobConstruction*>(jc)->ConstructJob(*this);
//     // entities.AddTopLevelJob(jb);
//   });
//   return Status();
// }
// Job *TopLevelJobEntitiesBuilder::BuildTopLevelJob(const JobConstruction *jc)
// {

//   return nullptr;
// }

// void TopLevelJobEntitiesBuilder::Finish() {}

Status Driver::BuildTopLevelJobConstructionEntities(
    TopLevelCompilationEntities &entities) {

  auto consumer = BuildingJobConstructionEntitiesConsumer::Create(*this);
  STONE_DEFER {
    [&]() {
      // Do something here
      consumer->Finish();
    }();
  };

  auto buildCompileInvocation =
      [&](BuildingJobConstructionEntitiesConsumer *consumer) -> Status {
    switch (GetCompileInvocationMode()) {
    case CompileInvocationMode::Multiple:
      return BuildMultipleCompileInvocation(consumer);
    case CompileInvocationMode::Single:
      return BuildSingleCompileInvocation(consumer);
    case CompileInvocationMode::Batch:
      return BuildBatchCompileInvocation(consumer);
    }
    // Work around MSVC warning: not all control paths return a value
    llvm_unreachable("All switch cases were covered");
  }(consumer);

  if (buildCompileInvocation.IsErrorOrHasCompletion()) {
    return Status::MakeHasCompletionAndIsError();
  }
}

Status Driver::BuildMultipleCompileInvocation(
    BuildingCompilationEntitiesConsumer *consumer) {

  assert(IsMultipleCompileInvocation());
  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          consumer->CompletedCompilationEntity(
              CreateCompileJobConstruction(input));
          break;
        }
        case FileType::Autolink:
        case FileType::Object: {
          consumer->CompletedCompilationEntity(input);
          break;
        }
        case FileType::StoneModuleFile:
        default:
          llvm_unreachable(" Invalid file type");
        }
      });
  return Status();
}

Status Driver::BuildSingleCompileInvocation(
    BuildingCompilationEntitiesConsumer *consumer) {

  assert(IsSingleCompileInvocation());
  auto compileJobConstruction = CreateCompileJobConstruction();

  assert(compileJobConstruction);

  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        assert(input->IsPartOfStoneCompilation());
        auto currentInput = llvm::dyn_cast<JobConstruction>(input);
        compileJobConstruction->AddInput(currentInput);
      });
  consumer->CompletedCompilationEntity(compileJobConstruction);
}

Status Driver::BuildBatchCompileInvocation(
    BuildingCompilationEntitiesConsumer *consumer) {}

CompileJobConstruction *
Driver::CreateCompileJobConstruction(const DriverInputFile *input) {

  // TODO: args.hasArg(opts::::EmbedBitCode))
  // Check that it requires a PCH

  if (input) {
    return CompileJobConstruction::Create(
        *this, input,
        GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
  }
  return CompileJobConstruction::Create(
      *this, GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
}

Status Driver::BuildTopLevelJobEntities(TopLevelCompilationEntities &entities) {

  // STONE_DEFER { jobEntitiesBuilder.Finish(); };
  // if (!entities.HasTopLevelJobConstructions()) {
  //   return Status::MakeHasCompletionAndIsError();
  // }
  // return jobEntitiesBuilder.BuildTopLevelJobEntities(entities);
}

Status Driver::BuildTopLevelCompilationEntities(
    TopLevelCompilationEntities &entities) {

  auto status = BuildTopLevelJobConstructionEntities(entities);
  if (status.IsErrorOrHasCompletion()) {
    return Status::MakeHasCompletionAndIsError();
  }
  // status = BuildTopLevelJobEntities(entities);
  // if (status.IsErrorOrHasCompletion()) {
  //   return Status::MakeHasCompletionAndIsError();
  // }
  return Status();
}

BuildingJobConstructionEntities::BuildingJobConstructionEntities(Driver &driver)
    : compileJobConstructionEntities(
          CompileJobConstructionEntities::Create(driver)) {}

Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  auto status =
      BuildTopLevelCompilationEntities(GetTopLevelCompilationEntities());

  return nullptr;
}

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
}

void Driver::PrintHelp(bool showHidden) const {

  unsigned IncludedFlagsBitmask = 0;
  unsigned ExcludedFlagsBitmask = opts::NoDriverOption;

  if (!showHidden) {
    ExcludedFlagsBitmask |= HelpHidden;
  }
  GetOptTable().printHelp(
      llvm::outs(), GetDriverOptions().GetMainExecutableName().data(),
      "Stone is a compiler tool for compiling Stone source code.",
      IncludedFlagsBitmask, ExcludedFlagsBitmask,
      /*ShowAllAliases*/ false);
}

//////////////////////

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
class TopLevelCompilationEntitiesBuilder;

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

class BuildingCompilationEntitiesConsumer
    : public DriverAllocation<BuildingCompilationEntitiesConsumer> {
protected:
  Driver &driver;

protected:
  llvm::SmallVector<const CompilationEntity *> entities;

public:
  BuildingCompilationEntitiesConsumer(Driver &driver);

protected:
  void AddCompilationEntity(const CompilationEntity *entity) {
    entities.push_back(entity);
  }
  bool HasCompilationEntities() {
    return (entities.empty() && entities.size() > 0);
  }
  /// Get each top level job
  void ForEachCompilationEntity(
      std::function<void(const CompilationEntity *entity)> callback);

public:
  BuildingCompilationEntitiesConsumer();

public:
  virtual void CompletedCompilationEntity(const CompilationEntity *entity);
  virtual void Finish();
};

class BuildingJobConstructionEntitiesConsumer final
    : public BuildingCompilationEntitiesConsumer {

public:
  BuildingJobConstructionEntitiesConsumer(Driver &driver);

public:
  void CompletedCompilationEntity(const CompilationEntity *entity) override;
  void Finish() override;

private:
  void CreateGeneratePCHJobConstruction();
  void CreateLinkJobConstruction();
  void CreateMergeModuleJobConstruction();
  void CreateAutolinkExtractJobConstruction();

public:
  static BuildingJobConstructionEntitiesConsumer *Create(Driver &driver);
};

// These are special, so we treat them as such
class CompileJobConstructionEntities final
    : public DriverAllocation<CompileJobConstructionEntities> {

  llvm::SmallVector<const CompilationEntity *, 8> entities;

public:
  explicit CompileJobConstructionEntities() {}

public:
  void AddCompilationEntity(const CompilationEntity *entity) {}

public:
  static CompileJobConstructionEntities *Create(const Driver &driver);
};

class LinkJobConstructionEntities final
    : public DriverAllocation<LinkJobConstructionEntities> {

  llvm::SmallVector<const CompilationEntity *, 8> entities;

public:
  explicit LinkJobConstructionEntities() {}

public:
  void AddCompilationEntity(const CompilationEntity *entity) {}

public:
  static LinkJobConstructionEntities *Create(const Driver &driver);
};

class BuildingJobConstructionEntities final
    : public DriverAllocation<BuildingJobConstructionEntities> {

  CompileJobConstructionEntities *compileJobConstructionEntities = nullptr;
  LinkJobConstructionEntities *linkJobConstructionEntities = nullptr;

public:
  BuildingJobConstructionEntities(Driver &driver);

public:
  CompileJobConstructionEntities *GetCompileJobConstructionEntities() {
    return compileJobConstructionEntities;
  }

public:
  void CreateGeneratePCHJobConstruction();
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
  static BuildingJobConstructionEntities *Create(Driver &driver);
};

// class MergeModuleJobConstructionEntitiesConsumer final
//     : public BuildingCompilationEntitiesConsumer {

// public:
//   MergeModuleJobConstructionEntitiesConsumer(Driver &driver);

// public:
//   void CompletedCompilationEntity(const CompilationEntity *entity) override;
//   void Finish() override;

// public:
//   static MergeModuleJobConstructionEntitiesConsumer *Create(Driver &driver);
// };

// class BuildingJobConstructionEntities final {

//   Driver &driver;
//   llvm::SmallVector<BuildingCompilationEntitiesConsumer *> consumers;

// public:
//   BuildingJobConstructionEntities(Driver &driver);

// public:
//   Status BuildForCompileInvocation(CompileInvocationMode kind);
//   Status BuildForMultipleCompileInvocation();
//   Status BuildForSingleCompileInvocation();
//   Status BuildForBatchCompileInvocation();

//   void ForEachConsumer(
//       std::function<void(BuildingCompilationEntitiesConsumer *consumer)> fn);

// public:
//   bool HasConsumers() { return (!consumers.empty() && consumers.size() > 0);
//   } void CompletedCompilationEntity(const CompilationEntity *entity);

//   CompileJobConstruction *
//   CreateCompileJobConstruction(const DriverInputFile *input = nullptr);

//   // void CompletedCompilationEntity(const DriverInputFile *entity);

// public:
//   void AddConsumer(BuildingCompilationEntitiesConsumer *consumer);

// public:
//   void Finish();
// };

// class JobEntitiesConsumer final : public BuildingCompilationEntitiesConsumer
// { public:
//   JobEntitiesConsumer(Driver &driver);

// public:
//   void CompletedCompilationEntity(const CompilationEntity *entity) override;
//   void Finish() override;

// public:
//   void AddConsumer(BuildingCompilationEntitiesConsumer *consumer);

// public:
//   static JobEntitiesConsumer *Create(Driver &driver);
// };

// class TopLevelJobEntitiesBuilder final {
//   Driver &driver;
//   llvm::SmallVector<BuildingCompilationEntitiesConsumer *> consumers;

// public:
//   TopLevelJobEntitiesBuilder(Driver &driver);

// public:
//   Status BuildTopLevelJobEntities(TopLevelCompilationEntities &entities);
//   Job *BuildTopLevelJob(const JobConstruction *jc);

// public:
//   void AddConsumer(BuildingCompilationEntitiesConsumer *consumer);
//   void Finish();
// };

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
  TopLevelCompilationEntities topLevelCompilationEntities;

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

  TopLevelCompilationEntities &GetTopLevelCompilationEntities() {
    return topLevelCompilationEntities;
  }
  const TopLevelCompilationEntities &GetTopLevelCompilationEntities() const {
    return topLevelCompilationEntities;
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

public:
  Status
  BuildTopLevelCompilationEntities(TopLevelCompilationEntities &entities);

public:
  Status
  BuildTopLevelJobConstructionEntities(TopLevelCompilationEntities &entities);

  Status BuildTopLevelJobEntities(TopLevelCompilationEntities &entities);

  Status
  BuildMultipleCompileInvocation(BuildingCompilationEntitiesConsumer *consumer);
  Status
  BuildSingleCompileInvocation(BuildingCompilationEntitiesConsumer *consumer);
  Status
  BuildBatchCompileInvocation(BuildingCompilationEntitiesConsumer *consumer);

  CompileJobConstruction *
  CreateCompileJobConstruction(const DriverInputFile *input = nullptr);

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

///// LATEST

class Driver::Implementation final {
  Driver &driver;

public:
  Implementation(Driver &driver);
  ~Implementation() = default;

public:
  using CompletedCompilationEntity =
      std::function<void(const CompilationEntity *entity)>;

  using CompletedMergeModule =
      std::function<void(const CompilationEntity *entity)>;

public:
  ///< Top level jcs
  MergeModuleJobConstruction *mergeModuleJobConstruction = nullptr;
  GeneratePCHJobConstruction *pchJobConstruction = nullptr;
  LinkJobConstruction *linkJobConstruction = nullptr;

public:
  llvm::SmallVector<const CompilationEntity *, 8> moduleInputs;
  llvm::SmallVector<const CompilationEntity *, 8> linkInputs;

  bool HasModuleInputs() {
    return !moduleInputs.empty() && moduleInputs.size() > 0;
  }
  bool HasLinkInputs() { return !linkInputs.empty() && linkInputs.size() > 0; }

public:
  void AddModuleInput(const CompilationEntity *entity);
  void AddLinkInput(const CompilationEntity *entity);

public:
  Status ActOnStoneFileType(const DriverInputFile *input = nullptr);
  Status ActOnObjectFileType(const DriverInputFile *input);
  Status ActOnAutoLinkFileType(const DriverInputFile *input);
  Status ActOnStoneModuleFileType(const DriverInputFile *input);

public:
  Status BuildTopLevelJobConstructions();

  CompileJobConstruction *
  CreateCompileJobConstruction(const DriverInputFile *input = nullptr);

  GeneratePCHJobConstruction *CreateGeneratePCHJobConstruction();

  bool ShouldMergeModule() {
    return (driver.ShouldGenerateModule() && HasModuleInputs());
  }
  Status MergeModule();
  MergeModuleJobConstruction *CreateMergeModuleJobConstruction();

  bool ShouldLink() { return (driver.ShouldLink() && HasLinkInputs()); }
  LinkJobConstruction *CreateLinkJobConstruction();
  BackendJobConstruction **BackendJobConstruction();

  bool ShouldModuleWrap();
  Status WrapModule(const CompilationEntity *mergeModule);
  ModuleWrapJobConstruction *CreateModuleWrapJobConstruction();

  bool ShouldPerformLTO() {
    driver.GetDriverOptions().GetDriverOutputInfo().HasLTO();
  }

  bool ShouldAutolinkExtract() {
    auto const triple = driver.GetDriverOptions().GetTriple();
    return (
        ((triple.getObjectFormat() == llvm::Triple::ELF && !triple.isPS4()) ||
         triple.getObjectFormat() == llvm::Triple::Wasm ||
         (triple.isOSCygMing()) && !ShouldPerformLTO()));
  }

public:
  Status BuildTopLevelJobs();
  Job *BuildJob(const JobConstruction *current);
  void ComputeJobMainOutput(const JobConstruction *jobConstruction);

public:
  Status BuildMultipleCompileInvocation();
  Status BuildSingleCompileInvocation();
  Status BuildBatchCompileInvocation();

public:
  Status FinishTopLevelJobConstructions();
};

Driver::Implementation::Implementation(Driver &driver) : driver(driver) {}

Status Driver::Implementation::BuildTopLevelJobConstructions() {

  STONE_DEFER { FinishTopLevelJobConstructions(); };
  switch (driver.GetCompileInvocationMode()) {
  case CompileInvocationMode::Multiple:
    return BuildMultipleCompileInvocation();
  case CompileInvocationMode::Single:
    return BuildSingleCompileInvocation();
  case CompileInvocationMode::Batch:
    return BuildBatchCompileInvocation();
  }
  llvm_unreachable("Invalid CompileInvocationMode!");
}

void Driver::Implementation::AddModuleInput(const CompilationEntity *entity) {
  if (auto incrementalEntity =
          llvm::dyn_cast<IncrementalJobConstruction>(entity)) {
  }
  moduleInputs.push_back(entity);
}

void Driver::Implementation::AddLinkInput(const CompilationEntity *entity) {
  linkInputs.push_back(entity);
}

Status
Driver::Implementation::ActOnStoneFileType(const DriverInputFile *input) {

  assert(input);
  assert(input->IsStoneFileType());
  auto compileJobConstruction = CreateCompileJobConstruction(input);

  // And the input to build the module
  AddModuleInput(compileJobConstruction);

  // Update the link entities
  AddLinkInput(compileJobConstruction);
}

Status Driver::Implementation::MergeModule() {
  assert(ShouldMergeModule());
  mergeModuleJobConstruction = CreateMergeModuleJobConstruction();
  return Status();
}

CompileJobConstruction *Driver::Implementation::CreateCompileJobConstruction(
    const DriverInputFile *input) {

  // TODO: args.hasArg(opts::::EmbedBitCode))
  // Check that it requires a PCH
  if (input) {
    return CompileJobConstruction::Create(driver, input,
                                          driver.GetOutputFileType());
  }
  return CompileJobConstruction::Create(driver, driver.GetOutputFileType());
}

MergeModuleJobConstruction *
Driver::Implementation::CreateMergeModuleJobConstruction() {
  return MergeModuleJobConstruction::Create(driver, moduleInputs);
}

LinkJobConstruction *Driver::Implementation::CreateLinkJobConstruction() {
  if (!linkJobConstruction) {
    if (driver.ShouldLink() && HasLinkInputs()) {
      if (driver.IsStaticLibraryLink()) {
        linkJobConstruction = StaticLinkJobConstruction::Create(
            driver, linkInputs, driver.GetLinkMode());
      } else {
        linkJobConstruction = DynamicLinkJobConstruction::Create(
            driver, linkInputs, driver.GetLinkMode(), ShouldPerformLTO());
      }
    }
  }
  return linkJobConstruction;
}

Status
Driver::Implementation::ActOnObjectFileType(const DriverInputFile *input) {
  if (driver.ShouldLink()) {
    AddLinkInput(input);
  }
}
Status
Driver::Implementation::ActOnAutoLinkFileType(const DriverInputFile *input) {
  if (driver.ShouldLink()) {
    AddLinkInput(input);
  }
}

Status
Driver::Implementation::ActOnStoneModuleFileType(const DriverInputFile *input) {

  if (driver.ShouldLink()) {
    AddLinkInput(input);
  } else if (driver.ShouldGenerateModule() && !driver.ShouldLink()) {
    AddModuleInput(input);
  } else {
    // TODO: Log
    Status::MakeHasCompletionAndIsError();
  }
  return Status();
}

bool Driver::Implementation::ShouldModuleWrap() {

  // if (MergeModuleAction) {
  //     if (OI.DebugInfoLevel == IRGenDebugInfoLevel::Normal) {
  //       const bool ModuleWrapRequired =
  //           Triple.getObjectFormat() != llvm::Triple::MachO;
  //       if (ModuleWrapRequired) {
  //         auto *ModuleWrapAction =
  //             C.createAction<ModuleWrapJobAction>(MergeModuleAction);
  //         LinkAction->addInput(ModuleWrapAction);
  //       } else {
  //         LinkAction->addInput(MergeModuleAction);
  //       }
  //       // FIXME: Adding the MergeModuleAction as top-level regardless would
  //       // allow us to get rid of the special case flag for that.
  //     } else {
  //       TopLevelActions.push_back(MergeModuleAction);
  //     }
  //   }
}

GeneratePCHJobConstruction *
Driver::Implementation::CreateGeneratePCHJobConstruction() {
  return nullptr;
}

Status Driver::Implementation::BuildMultipleCompileInvocation() {

  assert(driver.IsMultipleCompileInvocation());

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          if (ActOnStoneFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::Autolink:
          if (ActOnAutoLinkFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        case FileType::Object: {
          if (ActOnObjectFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::StoneModule:
          if (ActOnStoneModuleFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        default:
          llvm_unreachable(" Invalid file type");
        }
      });
  return Status();
}
Status Driver::Implementation::BuildSingleCompileInvocation() {

  auto compileJobConstruction = CreateCompileJobConstruction();
  assert(compileJobConstruction);

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        assert(input->HasValidFileType());
        compileJobConstruction->AddInput(input);
      });

  AddModuleInput(compileJobConstruction);
  AddModuleInput(compileJobConstruction);
}

Status Driver::Implementation::BuildBatchCompileInvocation() {}

Status Driver::Implementation::FinishTopLevelJobConstructions() {

  if (ShouldMergeModule()) {
    if (MergeModule().IsError()) {
      return Status::MakeHasCompletionAndIsError();
    }
  }
  //   // if(GetMergeModuleJobConstruction()){
  //   // }
  //   // TopLevelActions.push_back(MergeModuleAction);
  //   //    }
  //   //  }
  //   //  TopLevelActions.push_back(LinkAction);

  if (ShouldLink()) {
    auto linkJobConstruction = CreateLinkJobConstruction();
    if (linkJobConstruction) {
      linkJobConstruction->AddIsTopLevel();
      driver.AddTopLevelEntity(linkJobConstruction);
    }
  }
  return Status();
}

void Driver::ForEachTopLevelEntity(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto e : entities) {
    callback(e);
  }
}

Status Driver::Implementation::BuildTopLevelJobs() {
  driver.ForEachTopLevelEntity([&](const CompilationEntity *entity) {
    if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
      auto job = BuildJob(jc);
      job->AddIsTopLevel();
      assert(driver.HasCompilation());
      driver.GetCompilation()->AddTopLevelJob(job);
    }
  });
  return Status();
}
Job *Driver::Implementation::BuildJob(const JobConstruction *current) {

  Job *job = nullptr;

  // // 2. Build up the list of input jobs.
  // llvm::SmallVector<const CompilationEntity *, 4> inputs;
  // llvm::SmallVector<const Job *, 4> deps;

  // auto constructJob =
  //     std::make_unique<ConstructingJob>(current, driver.GetCompilation());

  for (const CompilationEntity *entity : *current) {
    if (entity->IsJobConstruction()) {
      if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {

        // jobInfo->deps.push_back(ConstructJob(jc));
      }
    } else if (entity->IsInput()) {
      // jobInfo->inputs.push_back(entity);
    }
  }

  return job;
}

void Driver::AddTopLevelEntity(const CompilationEntity *entity) {
  assert(entity);
  assert(entity->HasIsTopLevel());
  assert(entity->HasAllowTopLevel());
  entities.push_back(entity);
}

static void BuildWithStoneFileType() {}
static void BuildWithObjectFileType() {}
// static TopLevelCompillationEntityList*
// BuildTopLevelJobConstructions(CompileInvocation& compileInvocation){
//   compileInvocation.BuildJobConstructions();

// }
// TopLevelCompillationEntityList* Driver::BuildTopLevelJobConstructions(){

//   return nullptr;
// }

// BuildingJobConstructionEntities::BuildingJobConstructionEntities(Driver
// &driver)
//     : driver(driver), moduleEntities(driver), linkEntities(driver) {}

// void ModuleEntities::AddEntity(const CompilationEntity *entity) {

//   if (auto incrementalJobEntity =
//           llvm::dyn_cast<IncrementalJobConstruction>(entity)) {
//   }
//   entities.push_back(entity);
// }

// Status BuildingJobConstructionEntities::HandleStoneFileType(
//     const DriverInputFile *input) {

//   assert(input);
//   assert(input->IsStoneFileType());

//   CompileJobConstruction *compileJobConstruction = nullptr;
//   if (input) {
//     compileJobConstruction = CompileJobConstruction::Create(
//         driver, input, driver.GetOutputFileType());
//   } else {
//     // TODO: This is only true in Single
//     compileJobConstruction =
//         CompileJobConstruction::Create(driver, driver.GetOutputFileType());
//   }
//   moduleEntities.AddEntity(compileJobConstruction);

//   // Update the link entities
//   linkEntities.AddEntity(compileJobConstruction);

//   return Status();
// }

// Status BuildingJobConstructionEntities::HandleObjectFileType(
//     const DriverInputFile *input) {

//   assert(input);
//   assert(input->IsObjectFileType());

//   return Status();
// }

// Status BuildingJobConstructionEntities::HandleAutoLinkFileType(
//     const DriverInputFile *input) {
//   return Status();
// }

// Status BuildingJobConstructionEntities::HandleStoneModuleFileType(
//     const DriverInputFile *input) {

//   if (driver.ShouldLink()) {
//     linkEntities.AddEntity(input);
//   } else if (driver.ShouldGenerateModule() && !driver.ShouldLink()) {
//     moduleEntities.AddEntity(input);
//   } else {
//     // TODO: Log
//     Status::MakeHasCompletionAndIsError();
//   }
//   return Status();
// }

// GeneratePCHJobConstruction *
// BuildingJobConstructionEntities::GetGeneratePCHJobConstruction() {

//   // TODO: Just a starter
//   if (!pchJobConstruction) {
//     if (driver.GetDriverOptions().shouldGeneratePCH) {
//     }
//     // Check that we can create this
//   }
//   return pchJobConstruction;
// }

// MergeModuleJobConstruction *
// BuildingJobConstructionEntities::GetMergeModuleJobConstruction() {

//   //: TODO: Get this from the ModuleEntities
//   if (!mergeModuleJobConstruction && !driver.IsSingleCompileInvocation() &&
//       moduleEntities.HasEntities()) {
//     mergeModuleJobConstruction =
//         MergeModuleJobConstruction::Create(driver, moduleEntities.entities);
//   }
//   return mergeModuleJobConstruction;
// }

// CompileJobConstruction *
// BuildingJobConstructionEntities::CreateCompileJobConstruction(
//     const DriverInputFile *input) {

//   // TODO: args.hasArg(opts::::EmbedBitCode))
//   // Check that it requires a PCH
//   if (input) {
//     return CompileJobConstruction::Create(driver, input,
//                                           driver.GetOutputFileType());
//   }
//   return CompileJobConstruction::Create(driver, driver.GetOutputFileType());
// }

// void BuildingJobConstructionEntities::FinishBuilding() {

//   // if(GetMergeModuleJobConstruction()){
//   // }
//   // TopLevelActions.push_back(MergeModuleAction);
//   //    }
//   //  }
//   //  TopLevelActions.push_back(LinkAction);

//   if (linkEntities.HasEntities() && driver.ShouldLink()) {
//     auto linkJobConstruction = linkEntities.Apply();
//     if (linkJobConstruction) {
//       linkJobConstruction->AddIsTopLevel();
//       driver.GetTopLevelEntities().AddTopLevelJobConstruction(
//           linkJobConstruction);
//     }
//   }
// }

// ModuleEntities::ModuleEntities(Driver &driver) : driver(driver) {}

// LinkEntities::LinkEntities(Driver &driver) : driver(driver) {}

// LinkJobConstruction *LinkEntities::Apply() {
//   LinkJobConstruction *linkJobConstruction = nullptr;

//   if (driver.ShouldLink() && HasEntities()) {
//     if (driver.IsStaticLibraryLink()) {
//       linkJobConstruction = StaticLinkJobConstruction::Create(
//           driver, entities, driver.GetLinkMode());
//     } else {
//       linkJobConstruction = DynamicLinkJobConstruction::Create(
//           driver, entities, driver.GetLinkMode(),
//           driver.GetDriverOptions().GetDriverOutputInfo().HasLTO());
//     }
//   }
//   return linkJobConstruction;
// }

// void Driver::ForEachJobConstruction(
//     std::function<void(const JobConstruction *entity)> callback) {
//   for (auto jc : jcs) {
//     callback(jc);
//   }
// }
Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  if (!compilation) {
    Driver::Implementation implementation(*this);
    if (implementation.BuildTopLevelJobConstructions()
            .IsErrorOrHasCompletion()) {
      return nullptr;
    }
    if (implementation.BuildTopLevelJobs().IsErrorOrHasCompletion()) {
      return nullptr;
    }
    // Need compilation now to build the jobs
    compilation = Compilation::Create(*this);

    implementation.BuildTopLevelJobs();
  }
  return compilation;
}

sys::TaskQueue *Driver::BuildTaskQueue(const Compilation *compilation) {

  // unsigned numberOfParallelTasks = 1;

  // if (const Arg *A = GetDerivedArgList().getLastArg(opts::j)) {

  //   if (llvm::StringRef(A->getValue())
  //           .getAsInteger(10, numberOfParallelCommands)) {

  //     // Diags.diagnose(SourceLoc(), diag::error_invalid_arg_value,
  //     //                A->getAsString(ArgList), A->getValue());
  //     return nullptr;
  //   }
  // }
  // if (EnvironmentVariableRequestedMaximumDeterminism()) {
  //     NumberOfParallelCommands = 1;
  //     Diags.diagnose(SourceLoc(), diag::remark_max_determinism_overriding,
  //                    "-j");
  // }

  // const bool skipSubTaskExecution =
  //   ArgList.hasArg(opts::SkipSubTaskExecution,
  //                  opts::PrintDriverJobs);
  // if (skipSubTaskExecution) {
  //   return std::make_unique<sys::BlankTaskQueue>(NumberOfParallelCommands);
  // } else {
  //   return std::make_unique<sys::TaskQueue>(NumberOfParallelCommands,
  //                                            C.getStatsReporter());
  // }

  // taskQueue = TaskQueue::Create(*this,
  // GetDriverOptions().numberOfParallelTasks);
  return taskQueue;
}

// Status Driver::BuildTopLevelJobConstructionEntities(
//     TopLevelCompilationEntities &entities, CompileInvocationMode cim) {

//   /// Build the job constructions
//   BuildingJobConstructionEntities buildingJobConstructionEntities(*this);

//   STONE_DEFER { buildingJobConstructionEntities.FinishBuilding(); };
//   switch (cim) {
//   case CompileInvocationMode::Multiple:
//     return BuildMultipleCompileInvocation(entities,
//                                           buildingJobConstructionEntities);
//   case CompileInvocationMode::Single:
//     return BuildSingleCompileInvocation(entities,
//                                         buildingJobConstructionEntities);
//   case CompileInvocationMode::Batch:
//     return BuildBatchCompileInvocation(entities,
//                                        buildingJobConstructionEntities);
//   }
//   llvm_unreachable("Invalid CompileInvocationMode!");
// }

// Status Driver::BuildTopLevelJobEntities(TopLevelCompilationEntities
// &entities) {

//   entities.ForEachTopLevelJobConstruction([&](const CompilationEntity
//   *entity) {
//     if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
//       auto job = ConstructJob(jc);
//       job->AddIsTopLevel();
//       entities.AddTopLevelJob(job);
//     }
//   });
//   return Status();
// }
// // TODO: Continue here....
// Job *Driver::ConstructJob(const JobConstruction *current) {

//   auto jobInfo = JobInfo::Create(*this, current);

//   for (const CompilationEntity *entity : *current) {
//     if (entity->IsJobConstruction()) {
//       if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
//         jobInfo->deps.push_back(ConstructJob(jc));
//       }
//     } else if (entity->IsInput()) {
//       jobInfo->inputs.push_back(entity);
//     }
//   }

//   jobInfo->commandOutput =
//       std::make_unique<CommandOutput>(current->GetFileType());

//   auto job = GetToolChain().ConstructJob(GetCompilation(), jobInfo);

//   return job;
// }

// void Driver::ComputeJobMainOutput(const JobConstruction *jobConstruction) {}

// Status Driver::BuildMultipleCompileInvocation(
//     TopLevelCompilationEntities &entities,
//     BuildingJobConstructionEntities &buildingEntities) {

//   assert(IsMultipleCompileInvocation());
//   GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//       [&](const DriverInputFile *input) {
//         switch (input->GetFileType()) {
//         case FileType::Stone: {
//           assert(input->IsPartOfStoneCompilation());
//           if (buildingEntities.HandleStoneFileType(input)
//                   .IsErrorOrHasCompletion()) {
//             return Status::MakeHasCompletionAndIsError();
//           }
//           break;
//         }
//         case FileType::Autolink:
//           if (buildingEntities.HandleAutoLinkFileType(input)
//                   .IsErrorOrHasCompletion()) {
//             return Status::MakeHasCompletionAndIsError();
//           }
//           break;
//         case FileType::Object: {
//           if (buildingEntities.HandleObjectFileType(input)
//                   .IsErrorOrHasCompletion()) {
//             return Status::MakeHasCompletionAndIsError();
//           }
//           break;
//         }
//         case FileType::StoneModule:
//           if (buildingEntities.HandleStoneModuleFileType(input)
//                   .IsErrorOrHasCompletion()) {
//             return Status::MakeHasCompletionAndIsError();
//           }
//           break;
//         default:
//           llvm_unreachable(" Invalid file type");
//         }
//       });
//   return Status();
// }

// Status Driver::BuildSingleCompileInvocation(
//     TopLevelCompilationEntities &entities,
//     BuildingJobConstructionEntities &buildingEntities) {

//   auto compileJobConstruction =
//   buildingEntities.CreateCompileJobConstruction();
//   assert(compileJobConstruction);

//   GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//       [&](const DriverInputFile *input) {
//         compileJobConstruction->AddInput(input);
//       });

//   buildingEntities.GetModuleEntities().AddEntity(compileJobConstruction);
//   buildingEntities.GetLinkEntities().AddEntity(compileJobConstruction);
// }

// Status Driver::BuildBatchCompileInvocation(
//     TopLevelCompilationEntities &entities,
//     BuildingJobConstructionEntities &buildingEntities) {
//   return Status();
// }
