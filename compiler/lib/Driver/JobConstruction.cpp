#include "stone/Driver/JobConstruction.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"

using namespace stone;
using namespace stone::file;

IncrementatlJobConstruction::IncrementatlJobConstruction(
    JobConstructionKind kind, JobConstructionInputList inputs,
    FileType fileType)
    : JobConstruction(kind, llvm::None, fileType) {}

llvm::ArrayRef<const Job *>
IncrementatlJobConstruction::ConstructJobs(const Driver &driver) {

  return {nullptr};
}

CompileJobConstruction::CompileJobConstruction(FileType outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, llvm::None,
                                  outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
}

llvm::ArrayRef<const Job *>
CompileJobConstruction::ConstructJobs(const Driver &driver) {

  // TODO: Think about this
  assert(HasTopLevel() && "Only top-level job construction can create jobs!");
  JobConstructionPrettyStackTrace compileJobConstruction("build compile jobs",
                                                         this);

  return {nullptr};
}

CompileJobConstruction *
CompileJobConstruction::Create(Driver &driver, FileType outputFileType) {

  return new (driver) CompileJobConstruction(outputFileType);
}

CompileJobConstruction::CompileJobConstruction(JobConstructionInput input,
                                               FileType outputFileType)
    : IncrementatlJobConstruction(JobConstructionKind::Compile, input,
                                  outputFileType) {

  assert(file::IsOutputableFileType(outputFileType));
}

CompileJobConstruction *
CompileJobConstruction::Create(Driver &driver, JobConstructionInput input,
                               FileType outputFileType) {

  return new (driver) CompileJobConstruction(input, outputFileType);
}

MergeModuleJobConstruction::MergeModuleJobConstruction(
    JobConstructionInputList inputs)
    : IncrementatlJobConstruction(JobConstructionKind::MergeModule, inputs,
                                  FileType::StoneModule) {}

llvm::ArrayRef<const Job *>
MergeModuleJobConstruction::ConstructJobs(const Driver &driver) {

  return {nullptr};
}

LinkJobConstruction::LinkJobConstruction(JobConstructionKind kind,
                                         JobConstructionInputList inputs,
                                         FileType outputFileType,
                                         LinkMode linkMode)
    : JobConstruction(kind, inputs, outputFileType), linkMode(linkMode) {}

llvm::ArrayRef<const Job *>
LinkJobConstruction::ConstructJobs(const Driver &driver) {

  return {nullptr};
}

DynamicLinkJobConstruction::DynamicLinkJobConstruction(
    JobConstructionInputList inputs, LinkMode linkMode, bool withLTO)
    : LinkJobConstruction(JobConstructionKind::DynamicLink, inputs,
                          FileType::Image, linkMode),
      withLTO(withLTO) {

  assert((linkMode != LinkMode::None) && (linkMode != LinkMode::StaticLibrary));
}

DynamicLinkJobConstruction *
DynamicLinkJobConstruction::Create(Driver &driver,
                                   JobConstructionInputList inputs,
                                   LinkMode linkMode, bool withLTO) {

  return new (driver) DynamicLinkJobConstruction(inputs, linkMode, withLTO);
}

StaticLinkJobConstruction::StaticLinkJobConstruction(
    JobConstructionInputList inputs, LinkMode linkMode)
    : LinkJobConstruction(JobConstructionKind::StaticLink, inputs,
                          FileType::Image, linkMode) {
  assert(linkMode == LinkMode::StaticLibrary);
}

StaticLinkJobConstruction *StaticLinkJobConstruction::Create(
    Driver &driver, JobConstructionInputList inputs, LinkMode linkMode) {

  return new (driver) StaticLinkJobConstruction(inputs, linkMode);
}

BackendJobConstruction::BackendJobConstruction(JobConstructionInput input,
                                               FileType outputFileType,
                                               size_t inputIndex)
    : JobConstruction(JobConstructionKind::Backend, input, outputFileType),
      inputIndex(inputIndex) {

  assert(file::IsOutputableFileType(outputFileType));
}

llvm::ArrayRef<const Job *>
BackendJobConstruction::ConstructJobs(const Driver &driver) {

  return {nullptr};
}

GeneratePCHJobConstruction::GeneratePCHJobConstruction(
    JobConstructionInput input, llvm::StringRef persistentPCHDir)
    :

      JobConstruction(JobConstructionKind::GeneratePCH, input,
                      persistentPCHDir.empty() ? FileType::PCH
                                               : FileType::None),
      persistentPCHDir(persistentPCHDir) {}

llvm::ArrayRef<const Job *>
GeneratePCHJobConstruction::ConstructJobs(const Driver &driver) {

  return {nullptr};
}

void CompilationEntities::AddTopLevelJobConstruction(
    const JobConstruction *construction) {
  assert(construction);
  assert(construction->HasTopLevel());
  topLevelJobConstructions.push_back(construction);
}

void CompilationEntities::ForEachTopLevelJobConstruction(
    std::function<void(const JobConstruction *construction)> callback) {
  for (auto topLevelJobConstruction : topLevelJobConstructions) {
    callback(topLevelJobConstruction);
  }
}

class BuildingJobConstructions final {
  Driver &driver;
  llvm::SmallVector<JobConstructionInput, 2> moduleInputs;
  llvm::SmallVector<JobConstructionInput, 2> linkerInputs;

public:
  BuildingJobConstructions(Driver &driver) : driver(driver) {}
  ~BuildingJobConstructions() = default;

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
  Status BuildNormalCompileStyle();
  Status BuildSingleCompileStyle();
  Status BuildFlatCompileStyle();

public:
  CompileJobConstruction *
  BuildCompileJobConstruction(JobConstructionInput input);
  LinkJobConstruction *BuildLinkJobConstruction();
  Status BuildMergeModuleJobConstruction();
  Status BuildBackendJobConstruction();

public:
  Status FinishBuildJobConstructions();
};

/// Build the job-constructions
Status Driver::BuildJobConstructions() {
  BuildingJobConstructions buildingJobConstructions(*this);
  STONE_DEFER { buildingJobConstructions.FinishBuildJobConstructions(); };
  auto status = [&]() -> Status {
    switch (GetDriverOptions().GetCompileStyle()) {
    case CompileStyle::Normal:
      return buildingJobConstructions.BuildNormalCompileStyle();
    case CompileStyle::Single:
      return buildingJobConstructions.BuildSingleCompileStyle();
    case CompileStyle::Flat:
      return buildingJobConstructions.BuildFlatCompileStyle();
    default:
      llvm_unreachable("Invalid compile invocation kind");
    }
  }();
  return Status();
}

Status BuildingJobConstructions::BuildNormalCompileStyle() {
  assert(driver.GetDriverOptions().GetCompileStyle() == CompileStyle::Normal);

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile &input) {
        assert(file::IsPartOfCompilation(input.GetFileType()));
        JobConstructionInput currentInput =
            driver.CastToJobConstructionInput(input);
        switch (input.GetFileType()) {
        case FileType::Stone: {
          currentInput = BuildCompileJobConstruction(currentInput);
          AddModuleInput(currentInput);
          if (driver.GetDriverOptions().IsLinkableAction()) {
            AddLinkerInput(currentInput);
          }
          break;
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

Status BuildingJobConstructions::BuildSingleCompileStyle() {

  assert(driver.GetDriverOptions().GetCompileStyle() == CompileStyle::Single);

  auto compileJobConstruction = CompileJobConstruction::Create(
      driver, driver.GetDriverOptions().GetOutputFileType());

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile &input) {
        assert(file::IsPartOfCompilation(input.GetFileType()));
        auto currentInput = driver.CastToJobConstructionInput(input);
        compileJobConstruction->AddInput(currentInput);
        return Status();
      });

  if (driver.GetDriverOptions().IsCompileOnlyAction()) {
    compileJobConstruction->AddTopLevel();
  }
  AddModuleInput(compileJobConstruction);

  if (driver.GetDriverOptions().IsLinkableAction()) {
    AddLinkerInput(compileJobConstruction);
  }
  return Status();
}
Status BuildingJobConstructions::BuildFlatCompileStyle() {
  assert(driver.GetDriverOptions().GetCompileStyle() == CompileStyle::Flat);
  return Status();
}

CompileJobConstruction *BuildingJobConstructions::BuildCompileJobConstruction(
    JobConstructionInput input) {

  assert(driver.GetDriverOptions().IsCompilableAction() &&
         "The current action does not support job creation -- cannot proceed "
         "with compilation!");

  CompileJobConstruction *compileJobConstruction = nullptr;

  // if(driver.CastToJobConstruction(input))
  compileJobConstruction = CompileJobConstruction::Create(
      driver, input, driver.GetDriverOptions().GetOutputFileType());

  if (driver.GetDriverOptions().IsCompileOnlyAction()) {
    compileJobConstruction->AddTopLevel();
  }
  return compileJobConstruction;
}

LinkJobConstruction *BuildingJobConstructions::BuildLinkJobConstruction() {

  assert(driver.GetDriverOptions().IsLinkableAction() &&
         "The action does not support linking!");
  assert(HasLinkerInputs() && "Canot link without linking inputs!");

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
  llvm_unreachable("Unsupported link construction -- cannot continue with the "
                   "compilation process!");
}

Status BuildingJobConstructions::BuildBackendJobConstruction() {

  return Status();
}

Status BuildingJobConstructions::FinishBuildJobConstructions() {

  if (driver.GetDriverOptions().IsLinkableAction() && HasLinkerInputs()) {
    auto linkJobConstruction = BuildLinkJobConstruction();
    if (!linkJobConstruction) {
      return Status::MakeHasCompletionAndIsError();
    }
    linkJobConstruction->AddTopLevel();
    driver.GetCompilationEntities().AddTopLevelJobConstruction(
        linkJobConstruction);
    return Status();
  }
}

/// Print the list of Actions in a Compilation.
void Driver::PrintJobConstructions() const {}
void JobConstructionPrettyStackTrace::print(llvm::raw_ostream &OS) const {}
