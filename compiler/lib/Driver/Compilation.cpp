#include "stone/Driver/Compilation.h"
#include "stone/Basic/Defer.h"
#include "stone/Driver/Driver.h"
#include "stone/Driver/TaskQueue.h"

using namespace stone;

Compilation::Compilation(const Driver &driver) : driver(driver) {}

class Compilation::Implementation final {
public:
  Compilation &compilation;

  /// All jobs which have been scheduled for execution (whether or not
  /// they've finished execution), or which have been determined that they
  /// don't need to run.
  CommandSet scheduledCommands;

  /// A temporary buffer to hold commands that were scheduled but haven't been
  /// added to the Task Queue yet, because we might try batching them together
  /// first.
  CommandSetVector pendingExecution;

  /// Set of synthetic BatchJobs that serve to cluster subsets of jobs waiting
  /// in PendingExecution. Also used to identify (then unpack) BatchJobs back
  /// to their underlying non-Batch Jobs, when running a callback from
  /// TaskQueue.
  CommandSet batchJobs;

  /// All jobs which have finished execution or which have been determined
  /// that they don't need to run.
  CommandSet finishedCommands;

  /// A map from a Job to the commands it is known to be blocking.
  ///
  /// The blocked jobs should be scheduled as soon as possible.
  llvm::SmallDenseMap<const Job *, llvm::TinyPtrVector<const Job *>, 16>
      blockingCommands;

  /// A map from commands that didn't get to run to whether or not they affect
  /// downstream commands.
  ///
  /// Only intended for source files.
  llvm::SmallDenseMap<const Job *, bool, 16> unfinishedCommands;

  /// Jobs that incremental-mode has decided it can skip.
  CommandSet deferredCommands;

public:
  /// TaskQueue for execution.
  std::unique_ptr<TaskQueue> taskQueue;

  /// Cumulative result of PerformJobs(), accumulated from subprocesses.
  // int resultCode = EXIT_SUCCESS;

  /// True if any Job crashed.
  bool anyAbnormalExit = false;

public:
  Implementation(Compilation &compilation);
  ~Implementation() = default;

public:
  void ScheduleJobsBeforeBatching();
  void FormBatchJobsAndAddPendingJobsToTaskQueue();
  void RunTaskQueueToCompletion();
  void CheckForUnfinishedJobs();

public:
  void NoteBuilding(const Job *cmd, const bool willBeBuilding,
                    llvm::StringRef reason);

public:
  void RunJobs();
  CompilationResult FinishJobs();
};

Compilation::Implementation::Implementation(Compilation &compilation)
    : compilation(compilation) {
  ScheduleJobsBeforeBatching();
  FormBatchJobsAndAddPendingJobsToTaskQueue();
}

void Compilation::Implementation::ScheduleJobsBeforeBatching() {}

void Compilation::Implementation::FormBatchJobsAndAddPendingJobsToTaskQueue() {}

void Compilation::Implementation::RunTaskQueueToCompletion() {}

void Compilation::Implementation::CheckForUnfinishedJobs() {}

void Compilation::Implementation::RunJobs() {}

CompilationResult Compilation::Implementation::FinishJobs() {
  CheckForUnfinishedJobs();
  return CompilationResult();
}

CompilationResult Compilation::RunJobs() {

  Compilation::Implementation implementation(*this);
  STONE_DEFER { return implementation.FinishJobs(); };

  implementation.RunJobs();
}