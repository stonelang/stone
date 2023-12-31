#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::file;

class BuildJobConstructionsImpl final {
public:
  Driver &driver;
  llvm::SmallVector<JobConstructionInput, 2> moduleInputs;
  llvm::SmallVector<JobConstructionInput, 2> linkerInputs;

public:
  BuildJobConstructionsImpl(Driver &driver) : driver(driver) {}
  ~BuildJobConstructionsImpl() = default;

public:
  void AddModuleInput(JobConstructionInput input) {
    moduleInputs.push_back(input);
  }
  void AddLinkerInput(JobConstructionInput input) {
    linkerInputs.push_back(input);
  }
  bool HasLinkerInputs() {
    return (!linkerInputs.empty() && linkerInputs.size() > 0);
  }
  bool HasModuleInputs() {
    return (!moduleInputs.empty() && moduleInputs.size() > 0);
  }

public:
  Status BuildForNormalCompileInvocation();
  Status BuildForSingleCompileInvocation();
  Status BuildForFlatCompileIvocation();

public:
  CompileJobConstruction *
  BuildCompileJobConstruction(JobConstructionInput input);
  Status BuildMergeModuleJobConstruction();
  Status BuildLinkJobConstruction();
  Status BuildBackendJobConstruction();

public:
  Status FinishBuildJobConstructions();
};

Status BuildJobConstructionsImpl::BuildForNormalCompileInvocation() {

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile &input) {
        assert(file::IsPartOfCompilation(input.GetFileType()));
        JobConstructionInput currentInput =
            const_cast<DriverInputFile *>(&input);
        switch (input.GetFileType()) {
        case FileType::Stone: {
          currentInput = BuildCompileJobConstruction(currentInput);
        }
        case FileType::Object: {
          AddLinkerInput(currentInput);
          break;
        }
        default: {
          llvm_unreachable(" Invalid file type");
        }
        }
        return Status();
      });
  return Status();
}

Status BuildJobConstructionsImpl::BuildForSingleCompileInvocation() {

  return Status();
}

Status BuildJobConstructionsImpl::BuildForFlatCompileIvocation() {
  return Status();
}

CompileJobConstruction *BuildJobConstructionsImpl::BuildCompileJobConstruction(
    JobConstructionInput input) {

  assert(driver.GetDriverOptions().IsCompilableAction() &&
         "The current action does not support job creation -- cannot proceed "
         "with compilation!");

  auto compileJobConstruction = CompileJobConstruction::Create(
      driver, input, driver.GetDriverOptions().GetOutputFileType());

  AddModuleInput(compileJobConstruction);

  if (driver.GetDriverOptions().IsLinkableAction()) {
    AddLinkerInput(compileJobConstruction);
  }
  return compileJobConstruction;
}
Status BuildJobConstructionsImpl::BuildMergeModuleJobConstruction() {

  return Status();
}

Status BuildJobConstructionsImpl::BuildLinkJobConstruction() {

  /// driver.AddTopLevelJobConstruction();

  return Status();
}

Status BuildJobConstructionsImpl::BuildBackendJobConstruction() {

  return Status();
}

Status BuildJobConstructionsImpl::FinishBuildJobConstructions() {

  if (driver.GetDriverOptions().IsLinkableAction() && HasLinkerInputs()) {
  }

  //   if (driver.GetInvocation().ShouldLink() && HasLinkerInputs()) {
  //     JobConstruction *linkJobConstruction = nullptr;

  //     // Add the linker inputs
  //     switch (driver.GetInvocation().GetLinkMode()) {
  //     case LinkMode::StaticLibrary: {
  //       linkJobConstruction = StaticLinkJobConstruction::Create(
  //           driver, linkerInputs, driver.GetInvocation().GetLinkMode());
  //     }
  //     default: {
  //       // FIXME: WithLTO
  //       linkJobConstruction = DynamicLinkJobConstruction::Create(
  //           driver, linkerInputs, driver.GetInvocation().GetLinkMode(),
  //           driver.GetInvocation().GetDriverOptions().WithLTO());
  //     }
  //       AddTopLevelJobConstruction(linkJobConstruction);
  //     }
  //     // TODO:
  //     // On ELF platforms there's no built in autolinking mechanism, so we
  //     // pull the info we need from the .o files directly and pass them as an
  //     // argument input file to the linker.

  //   } else {
  //     // We can't rely on the merge module action being the only top-level
  //     // action that needs to run. There may be other actions (e.g.
  //     // BackendJobActions) that are not merge-module inputs but should be
  //     run
  //     // anyway.
  //     // if (MergeModuleAction){
  //     //   AddTopLevelJobConstruction(MergeModuleAction);
  //     // }
  //     // topLevelActions.append(AllLinkerInputs.begin(),
  //     AllLinkerInputs.end());
  //   }
}

/// Build the job-constructions
Status Driver::BuildJobConstructions() {

  BuildJobConstructionsImpl buildJobConstructionsImpl(*this);
  STONE_DEFER { buildJobConstructionsImpl.FinishBuildJobConstructions(); };

  auto status = [&]() -> Status {
    switch (GetDriverOptions().GetCompileInvocationMode()) {
    case CompileInvocationMode::Normal:
      return buildJobConstructionsImpl.BuildForNormalCompileInvocation();
    case CompileInvocationMode::Single:
      return buildJobConstructionsImpl.BuildForSingleCompileInvocation();
    case CompileInvocationMode::Flat:
      return buildJobConstructionsImpl.BuildForFlatCompileIvocation();
    default:
      llvm_unreachable("Invalid compilation kind");
    }
  }();
}
