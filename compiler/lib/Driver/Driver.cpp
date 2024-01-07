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
    : optTable(stone::CreateOptTable()), jobEntitiesBuilder(*this),
      jobConstructionEntitiesBuilder(*this) {}

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

TopLevelCompilationEntitiesConsumer::TopLevelCompilationEntitiesConsumer(
    Driver &driver)
    : driver(driver) {}

void TopLevelCompilationEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  llvm_unreachable("Only sub-classes can make this call");
}

void TopLevelCompilationEntitiesConsumer::Finish() {
  llvm_unreachable("Only sub-classes can make this call");
}

LinkJobConstructionEntitiesConsumer::LinkJobConstructionEntitiesConsumer(
    Driver &driver)
    : TopLevelCompilationEntitiesConsumer(driver) {

  assert(driver.GetDriverOptions().GetDriverOutputInfo().ShouldLink());
}

LinkJobConstructionEntitiesConsumer *
LinkJobConstructionEntitiesConsumer::Create(Driver &driver) {
  if (driver.GetDriverOptions().GetDriverOutputInfo().ShouldLink()) {
    return new (driver) LinkJobConstructionEntitiesConsumer(driver);
  }
  return nullptr;
}

void LinkJobConstructionEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {

  // asert(entity->IsTopLevel());

  /// TODO: Some checks
  AddTopLevelCompilationEntity(entity);
}

void LinkJobConstructionEntitiesConsumer::Finish() {

  if (HasTopLevelCompilationEntities()) {
    auto const outputInfo = driver.GetDriverOptions().GetDriverOutputInfo();
    if (outputInfo.IsStaticLibraryLink()) {

      driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
          StaticLinkJobConstruction::Create(driver, entities,
                                            outputInfo.GetLinkMode()));
    } else {
      driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
          DynamicLinkJobConstruction::Create(
              driver, entities, outputInfo.GetLinkMode(), outputInfo.HasLTO()));
    }
  }
}

MergeModuleJobConstructionEntitiesConsumer::
    MergeModuleJobConstructionEntitiesConsumer(Driver &driver)
    : TopLevelCompilationEntitiesConsumer(driver) {

  assert(!driver.IsSingleCompileInvocation());
  assert(driver.ShouldGenerateModule());
}

MergeModuleJobConstructionEntitiesConsumer *
MergeModuleJobConstructionEntitiesConsumer::Create(Driver &driver) {

  if (driver.IsSingleCompileInvocation()) {
    return nullptr;
  }

  if (!driver.ShouldGenerateModule()) {
    return nullptr;
  }

  return new (driver) MergeModuleJobConstructionEntitiesConsumer(driver);
}

void MergeModuleJobConstructionEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  // Add to the special Module

  if (auto incrementalJobEntity =
          llvm::dyn_cast<IncrementalJobConstruction>(entity)) {
    // Take the upper bound of the status of any incremental inputs to
    // ensure that the merge-modules job gets run if *any* input job is run.
    // const auto conservativeStatus =
    //     std::max(StatusBound.status, IJA->getInputInfo().status);
    // // The modification time here is not important to the rest of the
    // // incremental build. We take the upper bound in case an attempt to
    // // compare the swiftmodule output's mod time and any input files is
    // // made. If the compilation has been correctly scheduled, the
    // // swiftmodule's mod time will always strictly exceed the mod time of
    // // any of its inputs when we are able to skip it.
    // const auto conservativeModTime = std::max(
    //     StatusBound.previousModTime, IJA->getInputInfo().previousModTime);
    // StatusBound = InputInfo{conservativeStatus, conservativeModTime};
  }
}

void MergeModuleJobConstructionEntitiesConsumer::Finish() {

  // Ok, nowe we can try to create the link job
  // But, you have to create a special M
}

TopLevelJobConstructionEntitiesBuilder::TopLevelJobConstructionEntitiesBuilder(
    Driver &driver)
    : driver(driver) {}

void TopLevelJobConstructionEntitiesBuilder::AddConsumer(
    TopLevelCompilationEntitiesConsumer *consumer) {
  if (consumer) {
    consumers.push_back(consumer);
  }
}

Status TopLevelJobConstructionEntitiesBuilder::BuildForCompileInvocation(
    CompileInvocationMode compileStyle) {
  switch (compileStyle) {
  case CompileInvocationMode::Multiple:
    return BuildForMultipleCompileInvocation();
  case CompileInvocationMode::Single:
    return BuildForSingleCompileInvocation();
  case CompileInvocationMode::Batch:
    return BuildForBatchCompileInvocation();
  default:
    llvm_unreachable("Invalid compile invocation kind");
  }
}

CompileJobConstruction *
TopLevelJobConstructionEntitiesBuilder::CreateCompileJobConstruction(
    const DriverInputFile *input) {

  // if (args.hasArg(opts::::EmbedBitCode)) {
  // }
  /// Check that it requires a PCH
  CompileJobConstruction *compileJobConstruction = nullptr;
  if (input) {
    compileJobConstruction = CompileJobConstruction::Create(
        driver, input,
        driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());
  } else {
    compileJobConstruction = CompileJobConstruction::Create(
        driver,
        driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());

    if (driver.IsSingleCompileInvocation()) {
    }
  }
  if (!IsTopLvelJobConstruction()) {
    driver.GetTopLevelCompilationEntities().AddTopLevelJobConstruction(
        compileJobConstruction);
    return compileJobConstruction;
  } else {
    // Notify consumer
    CompletedCompilationEntity(compileJobConstruction);
  }
  return compileJobConstruction;
}

void TopLevelJobConstructionEntitiesBuilder::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  ForEachConsumer([&](TopLevelCompilationEntitiesConsumer *consumer) {
    consumer->CompletedCompilationEntity(entity);
  });
}

Status
TopLevelJobConstructionEntitiesBuilder::BuildForMultipleCompileInvocation() {
  assert(driver.IsMultipleCompileInvocation());

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        assert(input->IsPartOfStoneCompilation());

        switch (input->GetFileType()) {
        case FileType::Stone: {
          (void)CreateCompileJobConstruction(input);
          break;
        }
        case FileType::Object: {
          CompletedCompilationEntity(input);
          break;
        }
        default:
          llvm_unreachable(" Invalid file type");
        }
      });

  return Status();
}
Status
TopLevelJobConstructionEntitiesBuilder::BuildForSingleCompileInvocation() {
  assert(driver.IsSingleCompileInvocation());

  auto compileJobConstruction = CreateCompileJobConstruction();
  assert(compileJobConstruction);

  driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
      [&](const DriverInputFile *input) {
        assert(input->IsPartOfStoneCompilation());
        auto currentInput = driver.CastToJobConstruction(input);
        compileJobConstruction->AddInput(currentInput);
      });

  return Status();
}
Status
TopLevelJobConstructionEntitiesBuilder::BuildForBatchCompileInvocation() {
  assert(driver.IsBatchCompileInvocation());

  return Status();
}

void TopLevelJobConstructionEntitiesBuilder::ForEachConsumer(
    std::function<void(TopLevelCompilationEntitiesConsumer *consumer)> fn) {
  for (auto consumer : consumers) {
    fn(consumer);
  }
}

void TopLevelJobConstructionEntitiesBuilder::Finish() {}

TopLevelJobEntitiesBuilder::TopLevelJobEntitiesBuilder(Driver &driver)
    : driver(driver) {}

void TopLevelJobEntitiesBuilder::Finish() {}

Status Driver::BuildTopLevelCompilationEntities(
    TopLevelCompilationEntities &entities) {

  auto status = BuildTopLevelJobConstructionEntities(entities);
  if (status.IsErrorOrHasCompletion()) {
    return Status::MakeHasCompletionAndIsError();
  }
  status = BuildTopLevelJobEntities(entities);
  if (status.IsErrorOrHasCompletion()) {
    return Status::MakeHasCompletionAndIsError();
  }
  return Status();
}

Status Driver::BuildTopLevelJobConstructionEntities(
    TopLevelCompilationEntities &entities) {

  STONE_DEFER { jobConstructionEntitiesBuilder.Finish(); };

  jobConstructionEntitiesBuilder.AddConsumer(
      LinkJobConstructionEntitiesConsumer::Create(*this));

  jobConstructionEntitiesBuilder.AddConsumer(
      MergeModuleJobConstructionEntitiesConsumer::Create(*this));

  return jobConstructionEntitiesBuilder.BuildForCompileInvocation(GetCompileInvocationMode());
}
Status Driver::BuildTopLevelJobEntities(TopLevelCompilationEntities &entities) {


  STONE_DEFER { jobEntitiesBuilder.Finish(); };

  if (!entities.HasTopLevelJobConstructions()) {
    return Status::MakeHasCompletionAndIsError();
  }

  entities.ForEachTopLevelJobConstruction([&](const CompilationEntity *entity) {
    auto jc = llvm::dyn_cast<JobConstruction>(entity);

    // auto topLevelJob = driver.CastToJobConstruction(entity)->ConstructJob();

    // auto topLevelJob = llmv::dyn_cast<Job>
  });
}

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
