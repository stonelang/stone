#include "stone/Driver/Driver.h"
#include "stone/Basic/Defer.h"
#include "stone/Basic/FileType.h"
#include "stone/Diag/CoreDiagnostic.h"
#include "stone/Diag/DriverDiagnostic.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/Job.h"
#include "stone/Strings.h"

#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"

using namespace stone;
using namespace stone::file;
using namespace llvm::opt;

Driver::Driver()
    : optTable(stone::CreateOptTable()),
      topLevelEntities(new TopLevelCompilationEntities()) {}

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
    : driver(driver) {}

void BuildingJobConstructionEntities::Initialize() {

  // We just create the module entities because it is not a rare event --
  // statistically more likely.
  moduleEntities = ModuleEntities::Create(driver);

  // We just create the link entities because it is not a rare event --
  // statistically more likely.
  linkEntities = LinkEntities::Create(driver);

  // Try
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
  moduleEntities->AddEntity(compileJobConstruction);

  // TODO: Do you really just want to add even though we are not in a valid link
  // mode?
  linkEntities->AddEntity(compileJobConstruction);

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
    linkEntities->AddEntity(input);
  } else if (driver.GetDriverOptions()
                 .GetDriverOutputInfo()
                 .shouldGenerateModule &&
             !driver.ShouldLink()) {
    moduleEntities->AddEntity(input);
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
      moduleEntities->HasEntities()) {
    mergeModuleJobConstruction =
        MergeModuleJobConstruction::Create(driver, moduleEntities->entities);
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

void BuildingJobConstructionEntities::FinishBuilding() {}

BuildingJobConstructionEntities *
BuildingJobConstructionEntities::Create(Driver &driver) {
  return new (driver) BuildingJobConstructionEntities(driver);
}

ModuleEntities *ModuleEntities::Create(const Driver &driver) {
  return new (driver) ModuleEntities();
}

LinkEntities *LinkEntities::Create(const Driver &driver) {
  return new (driver) LinkEntities();
}

Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  auto status = BuildTopLevelJobConstructionEntities(
      GetTopLevelEntities(), GetCompileInvocationMode());

  return nullptr;
}

Status Driver::BuildTopLevelJobConstructionEntities(
    TopLevelCompilationEntities &entities, CompileInvocationMode cim) {

  auto buildingEntities = BuildingJobConstructionEntities::Create(*this);
  buildingEntities->Initialize();

  STONE_DEFER { buildingEntities->FinishBuilding(); };

  switch (cim) {
  case CompileInvocationMode::Multiple:
    return BuildMultipleCompileInvocation(entities, buildingEntities);
  case CompileInvocationMode::Single:
    return BuildSingleCompileInvocation(entities, buildingEntities);
  case CompileInvocationMode::Batch:
    return BuildBatchCompileInvocation(entities, buildingEntities);
  }
  llvm_unreachable("Invalid CompileInvocationMode!");
}

Status Driver::BuildMultipleCompileInvocation(
    TopLevelCompilationEntities &entities,
    BuildingJobConstructionEntities *buildingEntities) {

  assert(IsMultipleCompileInvocation());
  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        switch (input->GetFileType()) {
        case FileType::Stone: {
          assert(input->IsPartOfStoneCompilation());
          if (buildingEntities->HandleStoneFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::Autolink:
          if (buildingEntities->HandleAutoLinkFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        case FileType::Object: {
          if (buildingEntities->HandleObjectFileType(input)
                  .IsErrorOrHasCompletion()) {
            return Status::MakeHasCompletionAndIsError();
          }
          break;
        }
        case FileType::StoneModule:
          if (buildingEntities->HandleStoneModuleFileType(input)
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
    BuildingJobConstructionEntities *buildingEntities) {

  auto compileJobConstruction =
      buildingEntities->CreateCompileJobConstruction();
  assert(compileJobConstruction);

  GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        compileJobConstruction->AddInput(input);
      });

  buildingEntities->GetModuleEntities()->AddEntity(compileJobConstruction);
  buildingEntities->GetLinkEntities()->AddEntity(compileJobConstruction);
}

Status Driver::BuildBatchCompileInvocation(
    TopLevelCompilationEntities &entities,
    BuildingJobConstructionEntities *buildingEntities) {
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
