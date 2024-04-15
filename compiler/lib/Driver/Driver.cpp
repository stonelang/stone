#include "stone/Driver/Driver.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/FileType.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/Job.h"
#include "stone/Driver/TaskQueue.h"
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

void Driver::ForEachTopLevelJobConstruction(
    std::function<void(const CompilationEntity *entity)> callback) {
  for (auto topLevelJobConstruction : topLevelJobConstructions) {
    callback(topLevelJobConstruction);
  }
}

class ModuleInputs final {

  Driver &driver;

  llvm::SmallVector<const CompilationEntity *> entities;
  MergeModuleJobConstruction *mergeModuleJobConstruction = nullptr;

public:
  ModuleInputs(Driver &driver) : driver(driver) {}

public:
  void AddModuleInput(const CompilationEntity *entity);
  bool HasModuleInputs() { return !entities.empty() && entities.size() > 0; }
};

void ModuleInputs::AddModuleInput(const CompilationEntity *entity) {}

class LinkerInputs final {

  Driver &driver;
  llvm::SmallVector<const CompilationEntity *> entities;

public:
  LinkerInputs(Driver &driver) : driver(driver) {}

public:
  void AddLinkerInput(const CompilationEntity *entity);
  bool HasLinkInputs() { return !entities.empty() && entities.size() > 0; }
};
void LinkerInputs::AddLinkerInput(const CompilationEntity *entity) {}

class BuildingJobConstructions final {

  Driver &driver;
  ModuleInputs moduleInputs;
  LinkerInputs linkerInputs;

public:
  BuildingJobConstructions(Driver &driver)
      : driver(driver), moduleInputs(driver), linkerInputs(driver) {}

  Status FinishBuildingJobConstructions();

public:
  ModuleInputs &GetModuleInputs() { return moduleInputs; }
  LinkerInputs &GetLinkerInputs() { return linkerInputs; }
  Driver &GetDriver() { return driver; }

public:
  bool ShouldGenerateModule() { return false; }
  bool ShouldMergeModule() {
    return (driver.ShouldGenerateModule() &&
            GetModuleInputs().HasModuleInputs());
  }
  bool ShouldModuleWrap() { return false; }

  bool ShouldLink() {
    return (driver.ShouldLink() && GetLinkerInputs().HasLinkInputs());
  }
  bool ShouldPerformLTO() {
    driver.GetDriverOptions().GetDriverOutputInfo().HasLTO();
  }
  bool ShouldEmbedBitCode() { return false; }

public:
  bool ShouldGeneratePCH() { return false; }
  GeneratePCHJobConstruction *GetGeneratePCHJobConstruction() {
    return nullptr;
  }

public:
  Status BuildMultipleCompileInvocation();
  Status HandleStoneFileType(GeneratePCHJobConstruction *pch,
                             const DriverInputFile *input);

  Status HandleObjectFileType(const DriverInputFile *input);
  Status HandleAutoLinkFileType(const DriverInputFile *input);
  Status HandleStoneModuleFileType(const DriverInputFile *input);

public:
  Status BuildSingleCompileInvocation();
};

Status BuildingJobConstructions::BuildMultipleCompileInvocation() {

  auto pch = [&]() -> GeneratePCHJobConstruction * {
    if (ShouldMergeModule()) {
      return GetGeneratePCHJobConstruction();
    }
    return nullptr;
  }();

  assert(GetDriver().IsMultipleCompileInvocation());
  GetDriver().GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          if (HandleStoneFileType(pch, input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::Autolink:
          if (HandleAutoLinkFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        case FileType::Object: {
          if (HandleObjectFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::StoneModule:
          if (HandleStoneModuleFileType(input).IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        default:
          llvm_unreachable(" Invalid file type");
        }
      });
  return Status();
}

Status
BuildingJobConstructions::HandleStoneFileType(GeneratePCHJobConstruction *pch,
                                              const DriverInputFile *input) {

  assert(input->IsPartOfStoneCompilation());
  CompileJobConstruction *cjc = nullptr;
  if (ShouldEmbedBitCode()) {
    cjc = CompileJobConstruction::Create(GetDriver(), input, FileType::BC);
  } else {
    cjc = CompileJobConstruction::Create(GetDriver(), input,
                                         GetDriver().GetOutputFileType());
  }
  if (pch) {
    cjc->AddInput(pch);
  }
  GetModuleInputs().AddModuleInput(cjc);
  GetLinkerInputs().AddLinkerInput(cjc);
  return Status();
}
Status
BuildingJobConstructions::HandleObjectFileType(const DriverInputFile *input) {
  if (ShouldLink()) {
    GetLinkerInputs().AddLinkerInput(input);
  }
  return Status();
}
Status
BuildingJobConstructions::HandleAutoLinkFileType(const DriverInputFile *input) {
  if (ShouldLink()) {
    GetLinkerInputs().AddLinkerInput(input);
  }
  return Status();
}

Status BuildingJobConstructions::HandleStoneModuleFileType(
    const DriverInputFile *input) {
  if (ShouldGenerateModule() && !ShouldLink()) {
    // When generating a .swiftmodule as a top-level output (as
    // opposed to, for example, linking an image), treat .swiftmodule
    // files as inputs to a MergeModule action.
    GetModuleInputs().AddModuleInput(input);
  } else if (ShouldLink()) {
    // Otherwise, if linking, pass .swiftmodule files as inputs to the
    // linker, so that their debug info is available.
    GetLinkerInputs().AddLinkerInput(input);
  } else {
    // driver.GetDiags(SrcLoc(), diag::error_unexpected_input_file,
    // input->GetFileName());
    return Status::MakeHasCompletionAndIsError();
  }
  return Status();
}

Status BuildingJobConstructions::BuildSingleCompileInvocation() {

  auto cjc =
      CompileJobConstruction::Create(GetDriver(), driver.GetOutputFileType());
  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        if (!input->HasValidFileType()) {
          return Status::MakeHasCompletionAndIsError();
        }
        cjc->AddInput(input);
      });
  GetModuleInputs().AddModuleInput(cjc);
  GetLinkerInputs().AddLinkerInput(cjc);
  return Status();
}

Status Driver::BuildTopLevelJobConstructions() {

  BuildingJobConstructions buildingJobConstructions(*this);

  auto FinishTopLevelJobConstructions = [&](Status status) -> Status {
    if (status.IsErrorOrHasCompletion()) {
      return status;
    }

    if (buildingJobConstructions.ShouldMergeModule()) {
    }
  };

  switch (GetCompileInvocationMode()) {
  case CompileInvocationMode::Multiple:
    return FinishTopLevelJobConstructions(
        buildingJobConstructions.BuildMultipleCompileInvocation());
  case CompileInvocationMode::Single:
    return FinishTopLevelJobConstructions(
        buildingJobConstructions.BuildSingleCompileInvocation());
  case CompileInvocationMode::Batch:
    llvm_unreachable("Batch CompileInvocationMode is invalid here!");
  }
}

Status BuildingJobConstructions::FinishBuildingJobConstructions() {

  return Status();
}

class BuildingJobs final {
  Driver &driver;

public:
  BuildingJobs(Driver &driver) : driver(driver) {}

public:
  Status BuildTopLevelJobs();
  Job *BuildJob(const JobConstruction *current);
  // Job *BuildJob(CompilationEntityList inputs);
  void ComputeJobMainOutput(const JobConstruction *current);

  Job *GetJobFromCache();
  void CacheJob(const Job *job);
};

Status BuildingJobs::BuildTopLevelJobs() {

  driver.ForEachTopLevelJobConstruction([&](const CompilationEntity *entity) {
    if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
      auto job = BuildJob(jc);
      job->AddIsTopLevel();

      // assert(driver.HasCompilation());
      // driver.GetCompilation()->AddTopLevelJob(job);
    }
  });

  return Status();
}
Job *BuildingJobs::BuildJob(const JobConstruction *current) {

  auto jobContext =
      std::make_unique<JobContext>(current, driver.GetCompilation());

  for (const CompilationEntity *entity : *current) {
    if (entity->IsJobConstruction()) {
      if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
        jobContext->AddDep(BuildJob(jc));
      }
    } else if (entity->IsInput()) {
      jobContext->AddInput(entity);
    }
  }

  jobContext->commandOutput =
      std::make_unique<CommandOutput>(current->GetFileType());
  auto job = driver.GetToolChain().ConstructJob(std::move(jobContext));

  // driver.GetCompilation()->AddTopLevelJob(job);

  return job;
}

Status Driver::BuildTopLevelJobs() {

  BuildingJobs buildingJobs(*this);
  buildingJobs.BuildTopLevelJobs();

  GetCompilation()->AddTopLevelJob(nullptr);
}

Compilation *Driver::BuildCompilation(const ToolChain *toolChain) {

  assert(!HasCompilation());
  if (BuildTopLevelJobConstructions().IsError()) {
    return nullptr;
  }
  return compilation;
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
