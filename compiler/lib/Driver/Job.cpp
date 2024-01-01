#include "stone/Driver/Job.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

JobContext::JobContext(
    Compilation &compilation, llvm::ArrayRef<const Job *> inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    const CommandOutput &commandOutput)
    : compilation(compilation), inputs(inputs),
      inputConstructions(inputConstructions), commandOutput(commandOutput) {}

Job::Job(const JobConstruction &construction,
         llvm::SmallVectorImpl<const Job *> &&inputs)
    : constructionAndCondition(&construction, JobCondition::Always),
      inputs(std::move(inputs)) {}

Job *ToolChain::ConstructJob(
    const JobConstruction &construction, Compilation &compilation,
    llvm::SmallVectorImpl<const Job *> &&inputs,
    llvm::ArrayRef<const JobConstruction *> inputConstructions,
    std::unique_ptr<CommandOutput> commandOutput) const {

  JobContext jobContext{compilation, inputs, inputConstructions,
                        *commandOutput};

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

  return Job::Create(compilation.GetDriver(), construction, std::move(inputs));
}

Job *Job::Create(const Driver &driver, const JobConstruction &construction,
                 llvm::SmallVectorImpl<const Job *> &&inputs) {
  return new (driver) Job(construction, std::move(inputs));
}

void CompilationEntities::AddTopLevelJob(const Job *job) {
  assert(job);
  assert(job->HasTopLevel());
  topLevelJobs.push_back(job);
}

void CompilationEntities::AddTopLevelExternalJob(const Job *job) {
  assert(job);
  assert(job->HasTopLevel());
  topLevelExternalJobs.push_back(job);
}

void CompilationEntities::ForEachTopLevelJob(
    std::function<void(const Job *job)> callback) {
  for (auto topLevelJob : topLevelJobs) {
    callback(topLevelJob);
  }
}

void CompilationEntities::ForEachTopLevelExternalJob(
    std::function<void(const Job *job)> callback) {
  for (auto topLevelExternalJob : topLevelExternalJobs) {
    callback(topLevelExternalJob);
  }
}

class BuildingTopLevelJobs final {
  const Driver &driver;
  Driver::JobCacheMap jobCacheMap;

public:
  BuildingTopLevelJobs(const Driver &driver) : driver(driver) {}
  ~BuildingTopLevelJobs() = default;

public:
  Status BeginBuidlingTopLevelJobs();

public:
  Status FinishBuildingTopLevelJobs();
};

Status BuildingTopLevelJobs::FinishBuildingTopLevelJobs() { return Status(); }

Status Driver::BuildTopLevelJobs() {

  BuildingTopLevelJobs buildingTopLevelJobs(*this);
  STONE_DEFER { buildingTopLevelJobs.FinishBuildingTopLevelJobs(); };

  // GetCompilationEntities().ForEachTopLevelJobConstruction(
  //     [&](const JobConstruction *construction) {
  //       const_cast<JobConstruction *>(construction)->ConstructJobs(*this);
  //     });
}

void Driver::ComputeMainOutputForTopLevelJob(JobConstruction *jobConstruction) {

}

/// Print the list of Actions in a Compilation.
void Driver::PrintJobs() const {}

void JobPrettyStackTrace::print(llvm::raw_ostream &OS) const {}
