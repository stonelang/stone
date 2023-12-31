#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"

using namespace stone;
using namespace stone::file;

class BuildJobConstructionsImpl final {

protected:
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
  LinkJobConstruction *BuildLinkJobConstruction();

  Status BuildMergeModuleJobConstruction();
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

  /// TODO: You may want to check this or do you just want to pass to the
  /// compile
  if (driver.GetDriverOptions().IsLinkableAction()) {
    AddLinkerInput(compileJobConstruction);
  }
  return compileJobConstruction;
}
Status BuildJobConstructionsImpl::BuildMergeModuleJobConstruction() {

  return Status();
}

LinkJobConstruction *BuildJobConstructionsImpl::BuildLinkJobConstruction() {

  if (driver.GetDriverOptions().IsLinkableAction() && HasLinkerInputs()) {
    JobConstruction *linkJobConstruction = nullptr;
    // Add the linker inputs
    switch (driver.GetDriverOptions().GetLinkMode()) {
    case LinkMode::StaticLibrary:
      return StaticLinkJobConstruction::Create(
          driver, linkerInputs, driver.GetDriverOptions().GetLinkMode());
    default:
      // FIXME: Compute LTO in DriverInputsAndOutputs
      return DynamicLinkJobConstruction::Create(
          driver, linkerInputs, driver.GetDriverOptions().GetLinkMode(),
          driver.GetDriverOptions().HasLTO());
    }
  }
}

Status BuildJobConstructionsImpl::BuildBackendJobConstruction() {

  return Status();
}

Status BuildJobConstructionsImpl::FinishBuildJobConstructions() {

  if (driver.GetDriverOptions().IsLinkableAction() && HasLinkerInputs()) {
    auto linkJobConstruction = BuildLinkJobConstruction();
    if (!linkJobConstruction) {
      return Status::MakeHasCompletionAndIsError();
    }
    driver.AddTopLevelJobConstruction(linkJobConstruction);
    return Status();
  }
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
      llvm_unreachable("Invalid compile invocation kind");
    }
  }();
}
