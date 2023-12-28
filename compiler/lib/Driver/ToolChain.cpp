#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"


using namespace stone;

ToolChain::ToolChain(ToolChainKind kind) : kind(kind) {}

std::unique_ptr<Job> ToolChain::ConstructJob(
    const JobConstruction &jobConstruction, Compilation &compilation,
    llvm::SmallVectorImpl<const Job *> &&inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    std::unique_ptr<JobOutput> output, const DriverOptions &driverOpts) const {

  return nullptr;
}