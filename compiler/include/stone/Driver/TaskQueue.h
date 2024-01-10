#ifndef STONE_DRIVER_DRIVER_TASK_QUEUE_H
#define STONE_DRIVER_DRIVER_TASK_QUEUE_H

#include "stone/Driver/DriverAllocation.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

#include <queue>

namespace stone {

class Compilation;
class DriverStatsReporter;

class Task; // forward declared to allow for platform-specific implementations

using ProcessID = llvm::sys::procid_t;

/// Indicates how a TaskQueue should respond to the task finished event.
enum class TaskFinishedResponse {
  /// Indicates that execution should continue.
  ContinueExecution,
  /// Indicates that execution should stop (no new tasks will begin execution,
  /// but tasks which are currently executing will be allowed to finish).
  StopExecution,
};

class TaskQueue : public DriverAllocation<TaskQueue> {

  /// Tasks which have not begun execution.

  std::queue<Task *> tasks;

  /// The number of tasks to execute in parallel.
  unsigned numberOfParallelTasks;

  /// Optional place to count I/O and subprocess events.
  DriverStatsReporter *stats;

public:
  TaskQueue(unsigned numberOfParallelTasks = 0,
            DriverStatsReporter *stats = nullptr);

public:
  /// Adds a task to the TaskQueue.
  ///
  /// \param ExecPath the path to the executable which the task should execute
  /// \param Args the arguments which should be passed to the task
  /// \param Env the environment which should be used for the task;
  /// must be null-terminated. If empty, inherits the parent's environment.
  /// \param Context an optional context which will be associated with the task
  /// \param SeparateErrors Controls whether error output is reported separately
  virtual void AddTask(const char *ExecPath, llvm::ArrayRef<const char *> Args,
                       llvm::ArrayRef<const char *> Env = llvm::None,
                       void *Context = nullptr, bool SeparateErrors = false);

  /// A callback which will be executed when each task begins execution
  ///
  /// \param Pid the ProcessId of the task which just began execution.
  /// \param Context the context which was passed when the task was added
  using TaskBeganCallback =
      std::function<void(ProcessID processID, void *context)>;

  // using TaskFinishedCallback = std::function<TaskFinishedResponse(
  //     ProcessId Pid, int ReturnCode, StringRef Output, StringRef Errors,
  //     TaskProcessInformation ProcInfo, void *Context)>;

  /// Synchronously executes the tasks in the TaskQueue.
  ///
  /// \param Began a callback which will be called when a task begins
  /// \param Finished a callback which will be called when a task finishes
  /// \param Signalled a callback which will be called if a task exited
  /// abnormally due to a signal
  ///
  /// \returns true if all tasks did not execute successfully
  // virtual bool
  // ExecuteTask(TaskBeganCallback Began = TaskBeganCallback(),
  //         TaskFinishedCallback Finished = TaskFinishedCallback(),
  //         TaskSignalledCallback Signalled = TaskSignalledCallback());

public:
  /// Indicates whether TaskQueue supports buffering output on the
  /// current system.
  ///
  /// \note If this returns false, the TaskFinishedCallback passed
  /// to \ref execute will always receive an empty StringRef for output, even
  /// if the task actually generated output.
  static bool SupportsBufferingOutput();

  /// Indicates whether TaskQueue supports parallel execution on the
  /// current system.
  static bool SupportsParallelExecution();

public:
  /// \returns the maximum number of tasks which this TaskQueue will execute in
  /// parallel
  unsigned GetNumberOfParallelTasks() const;

public:
  static TaskQueue *Create(const Compilation *compilation);
};

// class EmptyTaskQueue : public TaskQueue {

// };

} // namespace stone
#endif