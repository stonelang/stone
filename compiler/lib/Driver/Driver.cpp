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
      compilationEntities(new CompilationEntities()) {}

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

// std::unique_ptr<CompileStyle> Driver::BuildCompileStyle() {
//   // Just return normal for now
//   return std::make_unique<NormalCompileStyle>(*this);
// }

// Status CompileStyle::BuildCompilationEntities(CompilationEntities &entities)
// {
//   llvm_unreachable("Illegal for for the base class CompileStyle");
// }

// CompileStyle::CompileStyle(Driver &driver) : driver(driver) {}

// NormalCompileStyle::NormalCompileStyle(Driver &driver) : CompileStyle(driver)
// {}

// Status
// NormalCompileStyle::BuildCompilationEntities(CompilationEntities &entities) {

//   driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//       [&](const DriverInputFile *input) {
//         // auto fileTypeExecution =
//         // driver.GetFileTypeExection(input.GetFileType());
//         // fileTypeExecution.Execute(entities);
//       });

//   return Status();
// }

// Status
// SingleCompileStyle::BuildCompilationEntities(CompilationEntities &entities) {

//   if (driver.GetDriverOptions().GetInputsAndOutputs().HasNoInputs()) {
//     return Status::MakeHasCompletionAndIsError();
//   }

//   auto jobConstruction = CompileJobConstruction::Create(
//       driver,
//       driver.GetDriverOptions().GetDriverOutputInfo().GetOutputFileType());

//   driver.GetDriverOptions().GetInputsAndOutputs().ForEachInput(
//       [&](const DriverInputFile *input) { jobConstruction->AddInput(input);
//       });

//   // Because this is a single you may be able to do this -- but, since you
//   are
//   // linking, maybe, the linker may be the top level job
//   auto jobs = jobConstruction->ConstructJobs(driver);

//   return Status();
// }

TopLevelEntitiesConsumer::TopLevelEntitiesConsumer(
    Driver &driver)
    : driver(driver) {}

void TopLevelEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  llvm_unreachable("Only sub-classes can make this call");
}

void TopLevelEntitiesConsumer::Finish() {
  llvm_unreachable("Only sub-classes can make this call");
}

LinkJobConstructionEntitiesConsumer::LinkJobConstructionEntitiesConsumer(
    Driver &driver)
    : TopLevelEntitiesConsumer(driver) {

  assert(driver.GetDriverOptions().GetDriverOutputInfo().HasLinkMode());
}

LinkJobConstructionEntitiesConsumer *
LinkJobConstructionEntitiesConsumer::Create(Driver &driver) {
  if (driver.GetDriverOptions().GetDriverOutputInfo().HasLinkMode()) {
    return new (driver) LinkJobConstructionEntitiesConsumer(driver);
  }
  return nullptr;
}

void LinkJobConstructionEntitiesConsumer::CompletedCompilationEntity(
    const CompilationEntity *entity) {

  /// TODO: Some checks
  AddCompilationEntity(entity);
}

void LinkJobConstructionEntitiesConsumer::Finish() {

  // Ok, now we can try to create the Link job
  // and add it to

  if (!HasCompilationEntities()) {
    return;
  }

  auto const outputInfo = driver.GetDriverOptions().GetDriverOutputInfo();

  if (outputInfo.HasStaticLibraryLinkMode()) {

    driver.GetCompilationEntities().AddTopLevelJobConstruction(
        StaticLinkJobConstruction::Create(driver, entities,
                                          outputInfo.GetLinkMode()));
  } else {
    driver.GetCompilationEntities().AddTopLevelJobConstruction(
        DynamicLinkJobConstruction::Create(
            driver, entities, outputInfo.GetLinkMode(), outputInfo.HasLTO()));
  }
}

MergeModuleJobConstructionEntitiesConsumer::
    MergeModuleJobConstructionEntitiesConsumer(Driver &driver)
    : TopLevelEntitiesConsumer(driver) {

  assert(
      !driver.GetDriverOptions().GetDriverOutputInfo().IsSingleCompileStyle());
  assert(
      driver.GetDriverOptions().GetDriverOutputInfo().ShouldGenerateModule());
}

MergeModuleJobConstructionEntitiesConsumer *
MergeModuleJobConstructionEntitiesConsumer::Create(Driver &driver) {

  if (driver.GetDriverOptions().GetDriverOutputInfo().IsSingleCompileStyle()) {
    return nullptr;
  }

  if (!driver.GetDriverOptions().GetDriverOutputInfo().ShouldGenerateModule()) {
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

JobConstructionEntitiesBuilder::JobConstructionEntitiesBuilder(Driver &driver)
    : driver(driver) {}

void JobConstructionEntitiesBuilder::AddConsumer(
    TopLevelEntitiesConsumer *consumer) {
  if (consumer) {
    consumers.push_back(consumer);
  }
}

Status JobConstructionEntitiesBuilder::BuildForCompileStyle(
    CompileStyleKind compileStyle) {
  switch (compileStyle) {
  case CompileStyleKind::Normal:
    return BuildForNormalCompileStyle();
  case CompileStyleKind::Single:
    return BuildForFlatCompileStyle();
  case CompileStyleKind::Flat:
    return BuildForSingleCompileStyle();
  default:
    llvm_unreachable("Invalid compile invocation kind");
  }
}

CompileJobConstruction *
JobConstructionEntitiesBuilder::CreateCompileJobConstruction(
    const DriverInputFile *input) {

  ///
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

    if (driver.GetDriverOptions()
            .GetDriverOutputInfo()
            .IsSingleCompileStyle()) {
    }
  }
  if (!IsTopLvelJobConstruction()) {
    driver.GetCompilationEntities().AddTopLevelJobConstruction(
        compileJobConstruction);
    return compileJobConstruction;
  } else {
    // Notify consumer
    CompletedCompilationEntity(compileJobConstruction);
  }
  return compileJobConstruction;
}

void JobConstructionEntitiesBuilder::CompletedCompilationEntity(
    const CompilationEntity *entity) {
  ForEachConsumer([&](TopLevelEntitiesConsumer *consumer) {
    consumer->CompletedCompilationEntity(entity);
  });
}

Status JobConstructionEntitiesBuilder::BuildForNormalCompileStyle() {

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
Status JobConstructionEntitiesBuilder::BuildForSingleCompileStyle() {

  assert(
      driver.GetDriverOptions().GetDriverOutputInfo().IsSingleCompileStyle());

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
Status JobConstructionEntitiesBuilder::BuildForFlatCompileStyle() {

  return Status();
}

void JobConstructionEntitiesBuilder::ForEachConsumer(
    std::function<void(TopLevelEntitiesConsumer *consumer)> fn) {
  for (auto consumer : consumers) {
    fn(consumer);
  }
}

JobEntitiesBuilder::JobEntitiesBuilder(Driver &driver) : driver(driver) {}

CompilationBuilder::CompilationBuilder(Driver &driver)
    : driver(driver), jobEntities(driver), jobConstructionEntities(driver) {}

Status CompilationBuilder::BuildCompilationEntities(
    CompilationEntities &entities) {

  jobConstructionEntities.AddConsumer(
      LinkJobConstructionEntitiesConsumer::Create(driver));

  jobConstructionEntities.AddConsumer(
      MergeModuleJobConstructionEntitiesConsumer::Create(driver));

  auto status = jobConstructionEntities.BuildForCompileStyle(
      driver.GetDriverOptions().GetDriverOutputInfo().GetCompileStyleKind());
}

void CompilationBuilder::Finish() {}

Status Driver::BuildCompilationEntities(CompilationEntities &entities) {

  CompilationBuilder buildingEntities(*this);
  STONE_DEFER { buildingEntities.Finish(); };

  return buildingEntities.BuildCompilationEntities(entities);
}

Compilation *Driver::BuildCompilation(const ToolChain &toolChain) {

  if (BuildCompilationEntities(GetCompilationEntities()).IsError()) {
    return nullptr;
  }

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
