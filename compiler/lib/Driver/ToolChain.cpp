#include "stone/Driver/ToolChain.h"
#include "stone/Driver/Compilation.h"

using namespace stone;

ToolChain::ToolChain(ToolChainKind kind, const Driver &driver)
    : kind(kind), driver(driver) {}

std::unique_ptr<Job> ToolChain::ConstructJob(
    const JobConstruction &construction, Compilation &compilation,
    llvm::SmallVectorImpl<const Job *> &&inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    std::unique_ptr<JobOutput> output, const DriverOptions &driverOpts) const {

  JobContext jobContext{compilation, inputs, inputConstructions, *output,
                        driverOpts};

  auto jobInvocation = [&]() -> JobInvocation {
    switch (construction.GetKind()) {
#define CASE(KIND)                                                             \
  case JobConstructionKind::KIND:                                              \
    return ConstructInvocation(cast<KIND##JobConstruction>(construction),      \
                               jobContext);
      CASE(Compile)
      CASE(Backend)
      CASE(DynamicLink)
      CASE(StaticLink)
#undef CASE
    case JobConstructionKind::None:
      llvm_unreachable("Job a JobConstruction");
    }
    // Work around MSVC warning: not all control paths return a value
    llvm_unreachable("All switch cases were covered");
  }();

  return nullptr;
}