#include "stone/Driver/Job.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

JobContext::JobContext(
    Compilation &compilation, llvm::ArrayRef<const Job *> inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    const JobOutput &jobOutput)
    : compilation(compilation), inputs(inputs),
      inputConstructions(inputConstructions), jobOutput(jobOutput) {}

Job::Job(const JobConstruction &construction)
    : constructionAndCondition(&construction, JobCondition::Always) {}

Job *ToolChain::ConstructJob(
    const JobConstruction &construction, Compilation &compilation,
    llvm::SmallVectorImpl<const Job *> &&inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    std::unique_ptr<JobOutput> output) const {

  JobContext jobContext{compilation, inputs, inputConstructions, *output};

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

  return new (compilation.GetDriver()) Job(construction);
}
