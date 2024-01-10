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
      constructor(constructor) {
  ClearJobFlags();
}

Job *Job::Create(const Driver &driver, const JobConstruction &constructor,
                 CompilationEntityList inputs) {

  return new (driver) Job(CompilationEntityKind::Job, constructor, inputs);
}

void Job::ClearJobFlags() {}
BatchJob::BatchJob(const JobConstruction &constructor)
    : BatchJob(constructor, llvm::None) {}

BatchJob::BatchJob(const JobConstruction &constructor,
                   CompilationEntityList inputs)
    : Job(CompilationEntityKind::BatchJob, constructor, inputs) {}

JobInfo *JobInfo::Create(Driver &driver, const JobConstruction *jc) {
  return new (driver) JobInfo(jc, driver.GetCompilation());
}

JobContext::JobContext(Compilation &compilation,
                       llvm::ArrayRef<const Job *> deps,
                       llvm::ArrayRef<const CompilationEntity *> inputs,
                       const CommandOutput &commandOutput)
    : compilation(compilation), deps(deps), inputs(inputs),
      commandOutput(commandOutput) {}

Job *ToolChain::ConstructJob(Compilation &compilation,
                             const JobInfo *jobInfo) const {

  JobContext jobContext{compilation, jobInfo->deps, jobInfo->inputs,
                        jobInfo->GetCommandOutput()};

  // TODO: use a template to get rid of the macro
  auto jobInvocation = [&]() -> JobInvocation {
    switch (jobInfo->GetJobConstruction()->GetKind()) {
#define CASE(K)                                                                \
  case CompilationEntityKind::K:                                               \
    return ConstructInvocation(llvm::cast<K>(*jobInfo->GetJobConstruction()),  \
                               jobContext);
      CASE(CompileJobConstruction)
      CASE(BackendJobConstruction)
      CASE(GeneratePCHJobConstruction)
      CASE(MergeModuleJobConstruction)
      CASE(DynamicLinkJobConstruction)
      CASE(StaticLinkJobConstruction)
      CASE(InterpretJobConstruction)
      CASE(AutolinkExtractJobConstruction)
#undef CASE
    case CompilationEntityKind::Input:
      llvm_unreachable("not a JobAction");
    }
    // Work around MSVC warning: not all control paths return a value
    llvm_unreachable("All switch cases are covered");
  }();
  // return Job::Create(compilation.GetDriver(),
  // construction,std::move(inputs));

  return nullptr;
}

CommandOutput::CommandOutput(file::FileType primaryOutputFileType)
    : primaryOutputFileType(primaryOutputFileType) {}
