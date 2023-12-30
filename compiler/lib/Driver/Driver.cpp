#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverAllocation.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver(DriverInvocation &invocation) : invocation(invocation) {}

Driver::~Driver() {}

Status Driver::Setup() {

  assert(invocation.HasAction());

  assert(invocation.GetToolChainKind() != ToolChainKind::None);
  toolChain = BuildToolChain(invocation.GetToolChainKind());

  compilation = BuildCompilation(invocation.GetCompilationKind());

  assert(HasCompilation());

  // if (compilation->BuildTopLevelJobConstructions().IsError()) {
  //   return Status::Error();
  // }

  // if (compilation->BuildJobs().IsError()) {
  //   return Status::Error();
  // }

  return Status();
}

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
}

std::unique_ptr<ToolChain> Driver::BuildToolChain(ToolChainKind kind) {
  switch (kind) {
  case ToolChainKind::Darwin:
    return std::make_unique<DarwinToolChain>(*this);
  // case ToolChainKind::Linux:
  //   return std::make_unique<LinuxToolChain>(*this);
  // case ToolChainKind::Windows:
  //   return std::make_unique<WindowsToolChain>(*this);
  // case ToolChainKind::FreeBSD:
  //   return std::make_unique<FreeBSDToolChain>(*this);
  // case ToolChainKind::OpenBSD:
  //   return std::make_unique<OpenBSDToolChain>(*this);
  // case ToolChainKind::Android:
  //   return std::make_unique<AndroidToolChain>(*this);
  // case ToolChainKind::Unix:
  //   return std::make_unique<UnixToolChain>(*this);
  default: {
    llvm_unreachable("Unsupported tool-chain");
  }
  }
}

std::unique_ptr<Compilation> Driver::BuildCompilation(CompilationKind kind) {

  assert(HasToolChain());
  BuildingCompilationRAII buildingCompilation(*this);
  switch (kind) {
  case CompilationKind::Normal:
    return BuildNormalCompilation(buildingCompilation);
  case CompilationKind::Flat:
    return BuildFlatCompilation(buildingCompilation);
  case CompilationKind::CPUCount:
    return BuildCPUCountCompilation(buildingCompilation);
  case CompilationKind::Single:
    return BuildSingleCompilation(buildingCompilation);
  default:
    llvm_unreachable(" Invalid compilation kind");
  }
}

std::unique_ptr<Compilation>
Driver::BuildNormalCompilation(BuildingCompilationRAII &buildingCompilation) {

  /// BuildingCompilationRAII buildingCompilation(*this);

  invocation.ForEachInputFile([&](InputFile &input) {
    JobConstructionInput currentInput = static_cast<InputFile *>(&input);

    switch (input.GetType()) {
    case file::Type::Stone: {

      assert(file::IsPartOfCompilation(input.GetType()));

      currentInput = CompileJobConstruction::Create(
          *this, currentInput,
          invocation.GetCompilationOptions().outputFileType);

      buildingCompilation.AddModuleInput(currentInput);

      // Basic for now
      buildingCompilation.AddLinkerInput(currentInput);
      break;
    }
    case file::Type::Object: {
      if (invocation.ShouldLink() && invocation.IsLinkOnly()) {
        buildingCompilation.AddLinkerInput(currentInput);
      }
      break;
    }
    default:
      llvm_unreachable(" Invalid file type");
    }
  });

  return nullptr;
}
std::unique_ptr<Compilation>
Driver::BuildFlatCompilation(BuildingCompilationRAII &buildingCompilation) {

  invocation.ForEachInputFile([&](InputFile &input) {
    JobConstructionInput currentInput = const_cast<InputFile *>(&input);
    currentInput = CompileJobConstruction::Create(
        *this, currentInput, invocation.GetCompilationOptions().outputFileType);

    buildingCompilation.AddModuleInput(currentInput);
    buildingCompilation.AddLinkerInput(currentInput);
  });

  return nullptr;
}

std::unique_ptr<Compilation>
Driver::BuildSingleCompilation(BuildingCompilationRAII &buildingCompilation) {

  auto compileJobConstruction = CompileJobConstruction::Create(
      *this, invocation.GetCompilationOptions().outputFileType);

  invocation.ForEachInputFile([&](InputFile &input) {
    JobConstructionInput currentInput = const_cast<InputFile *>(&input);
    compileJobConstruction->AddInput(currentInput);
  });

  buildingCompilation.AddModuleInput(compileJobConstruction);
  buildingCompilation.AddLinkerInput(compileJobConstruction);

  return nullptr;
}

std::unique_ptr<Compilation>
Driver::BuildCPUCountCompilation(BuildingCompilationRAII &buildingCompilation) {

  invocation.ForEachInputFile([&](InputFile &input) {

  });

  return nullptr;
}

Driver::BuildingCompilationRAII::~BuildingCompilationRAII() {

  if (driver.GetInvocation().ShouldLink() && HasLinkerInputs()) {
    JobConstruction *linkJobConstruction = nullptr;

    // Add the linker inputs
    switch (driver.GetInvocation().GetLinkMode()) {
    case LinkMode::StaticLibrary: {
      linkJobConstruction = StaticLinkJobConstruction::Create(
          driver, linkerInputs, driver.GetInvocation().GetLinkMode());
    }
    default: {
      // FIXME: WithLTO
      linkJobConstruction = DynamicLinkJobConstruction::Create(
          driver, linkerInputs, driver.GetInvocation().GetLinkMode(),
          driver.GetInvocation().GetCompilationOptions().WithLTO());
    }
      AddTopLevelJobConstruction(linkJobConstruction);
    }
    // TODO:
    // On ELF platforms there's no built in autolinking mechanism, so we
    // pull the info we need from the .o files directly and pass them as an
    // argument input file to the linker.

  } else {
    // We can't rely on the merge module action being the only top-level
    // action that needs to run. There may be other actions (e.g.
    // BackendJobActions) that are not merge-module inputs but should be run
    // anyway.
    // if (MergeModuleAction){
    //   AddTopLevelJobConstruction(MergeModuleAction);
    // }
    // topLevelActions.append(AllLinkerInputs.begin(), AllLinkerInputs.end());
  }
}

std::unique_ptr<stone::TaskQueue>
Driver::BuildTaskQueue(const Compilation &compilation) {

  return nullptr;
}

// Status Driver::BuildTopLevelJobConstructions(Compilation &compilation) {

//   return Status();
// }

// Status Driver::BuildTopLevelJobConstruction() {}

/// Build the job-constructions
// JobConstruction *Driver::CreateJobConstruction() { return nullptr; }

void Driver::ForEachJobConstruction(
    std::function<void(JobConstruction &construction)> callback) {}

Status Driver::BuildJobs() {}

/// Print the driver version.
void Driver::PrintVersion(const ToolChain &toolChain, raw_ostream &os) const {}

void Driver::PrintHelp(bool showHidden) const {

  unsigned IncludedFlagsBitmask = 0;
  unsigned ExcludedFlagsBitmask = opts::NoDriverOption;

  if (!showHidden) {
    ExcludedFlagsBitmask |= HelpHidden;
  }
  invocation.GetOptTable().printHelp(
      llvm::outs(), invocation.GetDriverOptions().mainExecutableName.data(),
      "Stone compiler", IncludedFlagsBitmask, ExcludedFlagsBitmask,
      /*ShowAllAliases*/ false);
}
