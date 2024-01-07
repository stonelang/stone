#include "stone/Driver/Job.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Compilation.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/DriverPrettyStackTrace.h"
#include "stone/Driver/ToolChain.h"

using namespace stone;

Job::Job(CompilationEntityKind kind, const JobConstruction &constructor,
         CompilationEntityList inputs)
    : TopLevelCompilationEntity(kind, inputs, file::FileType::None),
      constructor(constructor) {}

Job *Job::Create(const Driver &driver, const JobConstruction &constructor,
                 CompilationEntityList inputs) {

  return new (driver) Job(CompilationEntityKind::Job, constructor, inputs);
}

BatchJob::BatchJob(const JobConstruction &constructor, CompilationEntityList inputs)
    : Job(CompilationEntityKind::BatchJob, constructor, inputs) {}
