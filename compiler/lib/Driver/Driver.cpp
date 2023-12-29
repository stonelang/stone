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
  case CompilationKind::Quadratic:
    return BuildQuaraticCompilation(buildingCompilation);
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
Driver::BuildQuaraticCompilation(BuildingCompilationRAII &state) {

  invocation.ForEachInputFile([&](InputFile &input) {
    // auto jobConstruction = CreateJobConstruction(*InputArg, InputType);

    JobConstructionInput currentInput = const_cast<InputFile *>(&input);

    //   switch(input.GetType()){
    //   case file::Type::Stone:{

    //   }
    // default:
    //     llvm_unreachable(" Invalid file type");
    //   }
  });

  return nullptr;
}
std::unique_ptr<Compilation>
Driver::BuildFlatCompilation(BuildingCompilationRAII &state) {

  invocation.ForEachInputFile([&](InputFile &input) {

  });

  return nullptr;
}

std::unique_ptr<Compilation>
Driver::BuildSingleCompilation(BuildingCompilationRAII &state) {

  invocation.ForEachInputFile([&](InputFile &input) {
    JobConstructionInput currentInput = const_cast<InputFile *>(&input);
  });

  return nullptr;
}

std::unique_ptr<Compilation>
Driver::BuildCPUCountCompilation(BuildingCompilationRAII &state) {

  invocation.ForEachInputFile([&](InputFile &input) {

  });

  return nullptr;
}

Driver::BuildingCompilationRAII::~BuildingCompilationRAII() {

  // TODO: If there are any linking stuff, do it
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
