#include "stone/Driver/Driver.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/FileType.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/TaskQueue.h"
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

BuildingJobConstructionEntities::BuildingJobConstructionEntities(Driver &driver)
    : driver(driver), moduleEntities(driver), linkEntities(driver) {}

void ModuleEntities::AddEntity(const CompilationEntity *entity) {

  if (auto incrementalJobEntity =
          llvm::dyn_cast<IncrementalJobConstruction>(entity)) {
  }
  entities.push_back(entity);
}

Status BuildingJobConstructionEntities::HandleStoneFileType(
    const DriverInputFile *input) {

  assert(input);
  assert(input->IsStoneFileType());

  CompileJobConstruction *compileJobConstruction = nullptr;
  if (input) {
    compileJobConstruction = CompileJobConstruction::Create(
        driver, input, driver.GetOutputFileType());
  } else {
    // TODO: This is only true in Single
    compileJobConstruction =
        CompileJobConstruction::Create(driver, driver.GetOutputFileType());
  }
  moduleEntities.AddEntity(compileJobConstruction);

  // Update the link entities
  linkEntities.AddEntity(compileJobConstruction);

  return Status();
}

Status BuildingJobConstructionEntities::HandleObjectFileType(
    const DriverInputFile *input) {

  assert(input);
  assert(input->IsObjectFileType());

  return Status();
}

Status BuildingJobConstructionEntities::HandleAutoLinkFileType(
    const DriverInputFile *input) {
  return Status();
}

Status BuildingJobConstructionEntities::HandleStoneModuleFileType(
    const DriverInputFile *input) {

  if (driver.ShouldLink()) {
    linkEntities.AddEntity(input);
  } else if (driver.ShouldGenerateModule() && !driver.ShouldLink()) {
    moduleEntities.AddEntity(input);
  } else {
    // TODO: Log
    Status::MakeHasCompletionAndIsError();
  }
  return Status();
}

GeneratePCHJobConstruction *
BuildingJobConstructionEntities::GetGeneratePCHJobConstruction() {

  // TODO: Just a starter
  if (!pchJobConstruction) {
    if (driver.GetDriverOptions().shouldGeneratePCH) {
    }
    // Check that we can create this
  }
  return pchJobConstruction;
}

MergeModuleJobConstruction *
BuildingJobConstructionEntities::GetMergeModuleJobConstruction() {

  //: TODO: Get this from the ModuleEntities
  if (!mergeModuleJobConstruction && !driver.IsSingleCompileInvocation() &&
      moduleEntities.HasEntities()) {
    mergeModuleJobConstruction =
        MergeModuleJobConstruction::Create(driver, moduleEntities.entities);
  }
  return mergeModuleJobConstruction;
}

CompileJobConstruction *
BuildingJobConstructionEntities::CreateCompileJobConstruction(
    const DriverInputFile *input) {

  // TODO: args.hasArg(opts::::EmbedBitCode))
  // Check that it requires a PCH
  if (input) {
    return CompileJobConstruction::Create(driver, input,
                                          driver.GetOutputFileType());
  }
  return CompileJobConstruction::Create(driver, driver.GetOutputFileType());
}

void BuildingJobConstructionEntities::FinishBuilding() {

  // if(GetMergeModuleJobConstruction()){
  // }
  // TopLevelActions.push_back(MergeModuleAction);
  //    }
  //  }
  //  TopLevelActions.push_back(LinkAction);

  if (linkEntities.HasEntities() && driver.ShouldLink()) {
    auto linkJobConstruction = linkEntities.Apply();
    if (linkJobConstruction) {
      linkJobConstruction->AddIsTopLevel();
      driver.GetTopLevelEntities().AddTopLevelJobConstruction(
          linkJobConstruction);
    }
  }
}

ModuleEntities::ModuleEntities(Driver &driver) : driver(driver) {}
LinkEntities::LinkEntities(Driver &driver) : driver(driver) {}

LinkJobConstruction *LinkEntities::Apply() {
  LinkJobConstruction *linkJobConstruction = nullptr;

  if (driver.ShouldLink() && HasEntities()) {
    if (driver.IsStaticLibraryLink()) {
      linkJobConstruction = StaticLinkJobConstruction::Create(
          driver, entities, driver.GetLinkMode());
    } else {
      linkJobConstruction = DynamicLinkJobConstruction::Create(
          driver, entities, driver.GetLinkMode(),
          driver.GetDriverOptions().GetDriverOutputInfo().HasLTO());
    }
  }
  return linkJobConstruction;
}

Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  auto status = BuildTopLevelJobConstructionEntities(
      GetTopLevelEntities(), GetCompileInvocationMode());

  if (status.IsErrorOrHasCompletion()) {
    return nullptr;
  }

  status = BuildTopLevelJobEntities(GetTopLevelEntities());
  if (status.IsErrorOrHasCompletion()) {
    return nullptr;
  }

  return compilation;
}

sys::TaskQueue *Driver::BuildTaskQueue(const Compilation *compilation) {

  //unsigned numberOfParallelTasks = 1;

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

  //taskQueue = TaskQueue::Create(*this, GetDriverOptions().numberOfParallelTasks);
  return taskQueue;
}

Status Driver::BuildTopLevelJobConstructionEntities(
    TopLevelCompilationEntities &entities, CompileInvocationMode cim) {

  /// Build the job constructions
  BuildingJobConstructionEntities buildingJobConstructionEntities(*this);

  STONE_DEFER { buildingJobConstructionEntities.FinishBuilding(); };
  switch (cim) {
  case CompileInvocationMode::Multiple:
    return BuildMultipleCompileInvocation(entities,
                                          buildingJobConstructionEntities);
  case CompileInvocationMode::Single:
    return BuildSingleCompileInvocation(entities,
                                        buildingJobConstructionEntities);
  case CompileInvocationMode::Batch:
    return BuildBatchCompileInvocation(entities,
                                       buildingJobConstructionEntities);
  }
  llvm_unreachable("Invalid CompileInvocationMode!");
}

Status Driver::BuildTopLevelJobEntities(TopLevelCompilationEntities &entities) {

  entities.ForEachTopLevelJobConstruction([&](const CompilationEntity *entity) {
    if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
      auto job = ConstructJob(jc);
      job->AddIsTopLevel();
      entities.AddTopLevelJob(job);
    }
  });
  return Status();
}
// TODO: Continue here....
Job *Driver::ConstructJob(const JobConstruction *current) {

  auto jobInfo = JobInfo::Create(*this, current);

  for (const CompilationEntity *entity : *current) {
    if (entity->IsJobConstruction()) {
      if (auto *jc = llvm::dyn_cast<JobConstruction>(entity)) {
        jobInfo->deps.push_back(ConstructJob(jc));
      }
    } else if (entity->IsInput()) {
      jobInfo->inputs.push_back(entity);
    }
  }

  jobInfo->commandOutput =
      std::make_unique<CommandOutput>(current->GetFileType());

  auto job = GetToolChain().ConstructJob(GetCompilation(), jobInfo);

  return job;
}

void Driver::ComputeJobMainOutput(const JobConstruction *jobConstruction) {}

Status Driver::BuildMultipleCompileInvocation(
    TopLevelCompilationEntities &entities,
    BuildingJobConstructionEntities &buildingEntities) {

  assert(IsMultipleCompileInvocation());
  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          if (buildingEntities.HandleStoneFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::Autolink:
          if (buildingEntities.HandleAutoLinkFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        case FileType::Object: {
          if (buildingEntities.HandleObjectFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::StoneModule:
          if (buildingEntities.HandleStoneModuleFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        default:
          llvm_unreachable(" Invalid file type");
        }
      });
  return Status();
}

Status Driver::BuildSingleCompileInvocation(
    TopLevelCompilationEntities &entities,
    BuildingJobConstructionEntities &buildingEntities) {

  auto compileJobConstruction = buildingEntities.CreateCompileJobConstruction();
  assert(compileJobConstruction);

  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        compileJobConstruction->AddInput(input);
      });

  buildingEntities.GetModuleEntities().AddEntity(compileJobConstruction);
  buildingEntities.GetLinkEntities().AddEntity(compileJobConstruction);
}

Status Driver::BuildBatchCompileInvocation(
    TopLevelCompilationEntities &entities,
    BuildingJobConstructionEntities &buildingEntities) {
  return Status();
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
