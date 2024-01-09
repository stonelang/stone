#include "stone/Driver/Compilation.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/Task.h"

using namespace stone;

Compilation::Compilation(const Driver &driver) : driver(driver) {}

class Compilation::Implementation final {

  Compilation &compilation;

public:
  Implementation(Compilation &compilation);
  ~Implementation();

public:
  void ScheduleJobsBeforeBatching();
  void FormBatchJobsAndAddPendingJobsToTaskQueue();
  void RunTaskQueueToCompletion();
  void CheckForUnfinishedJobs();

public:
  void RunJobs();
};

Compilation::Implementation::Implementation(Compilation &compilation)
    : compilation(compilation) {}
Compilation::Implementation::~Implementation() {}

void Compilation::Implementation::ScheduleJobsBeforeBatching() {}

void Compilation::Implementation::FormBatchJobsAndAddPendingJobsToTaskQueue() {}

void Compilation::Implementation::RunTaskQueueToCompletion() {}

void Compilation::Implementation::CheckForUnfinishedJobs() {}

void Compilation::Implementation::RunJobs() {}

CompilationResult Compilation::RunJobs() {

  Compilation::Implementation implementation(*this);
  implementation.RunJobs();

  return CompilationResult();
}