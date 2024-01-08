#include "stone/Driver/Job.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Job::Job(CompilationEntityKind kind, const JobConstruction &constructor)
    : Job(kind, constructor, llvm::None) {}

Job::Job(CompilationEntityKind kind, const JobConstruction &constructor,
         CompilationEntityList inputs)
    : TopLevelCompilationEntity(kind, inputs, file::FileType::None),
      constructor(constructor) {}

Job *Job::Create(const Driver &driver, const JobConstruction &constructor,
                 CompilationEntityList inputs) {

  return new (driver) Job(CompilationEntityKind::Job, constructor, inputs);
}

BatchJob::BatchJob(const JobConstruction &constructor)
    : BatchJob(constructor, llvm::None) {}

BatchJob::BatchJob(const JobConstruction &constructor,
                   CompilationEntityList inputs)
    : Job(CompilationEntityKind::BatchJob, constructor, inputs) {}

JobInfo *JobInfo::Create(Driver &driver, const JobConstruction *jobConstruction,
                         Compilation &compilation) {
  return new (driver) JobInfo(jobConstruction, compilation);
}


JobContext::JobContext(Compilation &compilation, llvm::ArrayRef<const Job *> deps,
                                  llvm::ArrayRef<const CompilationEntity *> inputs,
                                  const CommandOutput &commandOutput)
    : compilation(compilation), deps(deps), inputs(inputs), commandOutput(commandOutput) {}



Job *ToolChain::ConstructJob(Compilation& compilation, const JobInfo *jobInfo) const {

  JobContext jobContext{compilation, jobInfo->deps,
                        jobInfo->inputs, jobInfo->GetCommandOutput()};

  auto jobInvocation = [&]() -> JobInvocation {
    switch (jobInfo->GetJobConstruction()->GetKind()) {
    case CompilationEntityKind::CompileJobConstruction:
      return ConstructInvocation(
          llvm::cast<CompileJobConstruction>(*jobInfo->GetJobConstruction()),
          jobContext);

    case CompilationEntityKind::Input:
      llvm_unreachable("not a JobConstruction");
    }

    // Work around MSVC warning: not all control paths return a value
    llvm_unreachable("All switch cases are covered");
  }();
  // return Job::Create(compilation.GetDriver(),
  // construction,std::move(inputs));

  return nullptr;
}

// Job *ToolChain::ConstructJob(
//     const JobConstruction &construction, Compilation &compilation,
//     llvm::SmallVectorImpl<const Job *> &&inputs,
//     llvm::ArrayRef<const JobConstruction *> inputConstructions,
//     std::unique_ptr<CommandOutput> commandOutput) const {

//   JobContext jobContext{compilation, inputs, inputConstructions,
//                         *commandOutput};

//   auto jobInvocation = [&]() -> JobInvocation {
//     switch (construction.GetKind()) {
// #define CASE(KIND) \
//   case CompilationEntityKind::KIND: \
//     return ConstructInvocation(cast<KIND##JobConstruction>(construction), \
//                                jobContext);
//       CASE(Compile)
//       CASE(Backend)
//       CASE(DynamicLink)
//       CASE(StaticLink)

// #undef CASE
//     case JobConstructionKind::None:
//       llvm_unreachable("Job a JobConstruction");
//     }
//     // Work around MSVC warning: not all control paths return a value
//     llvm_unreachable("All switch cases were covered");
//   }();

//   return Job::Create(compilation.GetDriver(), construction,
//   std::move(inputs));
// }

// Job *Job::ConstructJob(
//     const JobConstruction &jobConstruction, Compilation &compilation,
//     llvm::ArrayRef<const Job *> jobEntities,
//     llvm::ArrayRef<const CompilationEntity *> inputEntities) {}

CommandOutput::CommandOutput(file::FileType primaryOutputFileType)
    : primaryOutputFileType(primaryOutputFileType) {}
