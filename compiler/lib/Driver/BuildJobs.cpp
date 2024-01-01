#include "stone/Basic/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

// class BuildJobsState final {
//   const Driver &driver;
//   JobCacheMap jobCache;

// public:
//   BuildJobsState(const Driver &driver) : driver(driver) {}
//   ~BuildJobsState() = default;

// public:
//   void BuildJobs(JobConstructionsState &jobConstructionsState) {

//     //  driver.ForEachJobConstruction([&](JobConstruction &construction) {

//     // });
//   }
//   void BuildJobsForJobConstruction() {}
// };

class BuildJobsImpl final {
  const Driver &driver;
  Driver::JobCacheMap jobCache;

public:
  BuildJobsImpl(const Driver &driver) : driver(driver) {}
  ~BuildJobsImpl() = default;

public:
  Status FinishBuildJobs();
};

Status BuildJobsImpl::FinishBuildJobs() {}

Status Driver::BuildTopLevelJobs() {

  GetCompilationEntities().ForEachTopLevelJobConstruction(
      [&](const JobConstruction *construction) {
        const_cast<JobConstruction *>(construction)->ConstructJobs(*this);
      });

  BuildJobsImpl buildJobsImpl(*this);
  STONE_DEFER { buildJobsImpl.FinishBuildJobs(); };
}
