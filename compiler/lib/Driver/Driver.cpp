#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverAllocation.h"

using namespace stone;

using namespace llvm::opt;

Driver::Driver() : invocation(*this) {}

Driver::~Driver() {}

Status Driver::Setup() { return Status(); }

void *stone::AllocateInDriver(size_t bytes, const stone::Driver &driver,
                              unsigned alignment) {
  return driver.Allocate(bytes, alignment);
}

std::unique_ptr<ToolChain>
Driver::BuildToolChain(const llvm::opt::InputArgList &inputArgList) {

  return nullptr;
}

std::unique_ptr<stone::TaskQueue>
Driver::BuildTaskQueue(const Compilation &compilation) {

  return nullptr;
}

Status Driver::BuildTopLevelJobConstructions() {

  switch (invocation.GetCompilationKind()) {
  case CompilationKind::Quadratic:
    break;
  case CompilationKind::Flat:
    break;
  case CompilationKind::CPUCount:
    break;
  case CompilationKind::Single:
    break;
  default:
    llvm_unreachable(
        "Cannot build top-level job-constructions -- invalid compilation kind");
  }
  return Status();
}

Status Driver::BuildTopLevelJobConstruction() {}

/// Build the job-constructions
JobConstruction *Driver::CreateJobConstruction() { return nullptr; }

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
