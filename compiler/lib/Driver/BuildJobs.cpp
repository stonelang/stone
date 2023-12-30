#include "stone/Basic/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"

using namespace stone;

// class BuildJobConstructionsState final {
// public:
//   const Driver &driver;

//   llvm::SmallVector<JobConstructionInput, 2> moduleInputs;
//   llvm::SmallVector<JobConstructionInput, 2> linkerInputs;

// protected:
//   void AddModuleInput(const JobConstructionInput input) {
//     moduleInputs.push_back(input);
//   }

//   void AddLinkerInput(const JobConstructionInput input) {
//     linkerInputs.push_back(input);
//   }

//   bool HasLinkerInputs() { return !linkerInputs.empty(); }

// public:
//   BuildJobConstructionsState(const Driver &driver) : driver(driver) {}

// public:
//   virtual Status BuildJobConstructions() = 0;

// public:
//   Status FinishBuildJobConstructions() {

//     if (driver.ShouldLink() && HasLinkerInputs()) {
//       JobConstruction *linkJobConstruction = nullptr;

//       // Add the linker inputs
//       switch (driver.GetInvocation().GetLinkMode()) {
//       case LinkMode::StaticLibrary: {
//         linkJobConstruction = StaticLinkJobConstruction::Create(
//             driver, linkerInputs,
//             driver.GetDriverOptions().GetLinkMode());
//       }
//       default: {
//         // FIXME: WithLTO
//         linkJobConstruction = DynamicLinkJobConstruction::Create(
//             driver, linkerInputs,
//             driver.GetDriverOptions().GetLinkMode(),
//             driver.GetDriverOptions().WithLTO());
//       }
//         driver.AddTopLevelJobConstruction(linkJobConstruction);
//       }
//       // TODO:
//       // On ELF platforms there's no built in autolinking mechanism, so we
//       // pull the info we need from the .o files directly and pass them as an
//       // argument input file to the linker.

//     } else {
//       // We can't rely on the merge module action being the only top-level
//       // action that needs to run. There may be other actions (e.g.
//       // BackendJobActions) that are not merge-module inputs but should be
//       run
//       // anyway.
//       // if (MergeModuleAction){
//       //   AddTopLevelJobConstruction(MergeModuleAction);
//       // }
//       // topLevelActions.append(AllLinkerInputs.begin(),
//       // AllLinkerInputs.end());
//     }
//     return Status();
//   }
// };

// class NormalJobConstructionsState final : public JobConstructionsState {
// public:
//   NormalJobConstructionsState(const Driver &driver)
//       : JobConstructionsState(driver) {}

//   ~NormalJobConstructionsState() = default;

// public:
//   Status BuildJobConstructions() override { return Status(); }
// };

// class SingleJobConstructionsState final : public JobConstructionsState {
// public:
//   SingleJobConstructionsState(const Driver &driver)
//       : JobConstructionsState(driver) {}
//   ~SingleJobConstructionsState() = default;

// public:
//   Status BuildJobConstructions() override {

//     auto compileJobConstruction = CompileJobConstruction::Create(
//         *this, driver.GetDriverOptions().outputFileType);

//     driver.ForEachInputFile([&](InputFile &input) {
//       JobConstructionInput currentInput = const_cast<InputFile *>(&input);
//       compileJobConstruction->AddInput(currentInput);
//     });

//     AddModuleInput(compileJobConstruction);
//     AddLinkerInput(compileJobConstruction);

//     return Status();
//   }
// };

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
public:
  Status FinishBuildJobs();
};

Status Driver::BuildJobs() {

  BuildJobsImpl buildJobsImpl(*this);
  STONE_DEFER { buildJobsImpl.FinishBuildJobs(); };

  // auto GetJobConstructionsState =
  //     [&](const Driver &driver) -> std::unique_ptr<JobConstructionsState> {
  //   switch (GetCompilationKind()) {
  //   case CompilationKind::Normal:
  //     return std::make_unique<NormalJobConstructionsState>(driver);
  //   case CompilationKind::Single:
  //     return std::make_unique<SingleJobConstructionsState>(driver);
  //   default:
  //     llvm_unreachable("Invalid compilation kind");
  //   }
  // };
  // auto topLevelJobConstructionsState = GetJobConstructionsState(*this);
  // topLevelJobConstructionsState->BuildJobConstructions();
  // topLevelJobConstructionsState->FinishJobConstructions();

  // BuildJobsState state(*this);
  // return state.BuildJobs(*topLevelJobConstructionsState);
}
