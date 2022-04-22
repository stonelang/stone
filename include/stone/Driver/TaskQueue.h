#ifndef STONE_COMPILER_TASKQUEUE_H
#define STONE_COMPILER_TASKQUEUE_H

#include <functional>
#include <memory>
#include <queue>

#include "stone/Basic/Context.h"
#include "stone/Basic/JSONSerialization.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/Task.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {

class TaskQueue;
class TaskQueueStats final : public Stats {
  const TaskQueue &queue;

public:
  TaskQueueStats(const TaskQueue &queue)
      : Stats("task-queue statistics:"), queue(queue) {}
  void Print(ColorfulStream &stream) override;
};

using ProcID = llvm::sys::procid_t;
enum class TaskQueueKind { None, Darwin, Unix, Win };

/// Indicates how a TaskQueue should respond to the task finished event.
enum class TaskFinishedResponse {
  /// Indicates that execution should continue.
  ContinueExecution,
  /// Indicates that execution should stop (no new tasks will begin execution,
  /// but tasks which are currently executing will be allowed to finish).
  StopExecution,
};

struct TaskProcessInfo {

  struct ResourceUsage {
    // user time in µs
    uint64_t Utime;
    // system time in µs
    uint64_t Stime;
    // maximum resident set size in Bytes
    uint64_t Maxrss;

    ResourceUsage(uint64_t Utime, uint64_t Stime, uint64_t Maxrss)
        : Utime(Utime), Stime(Stime), Maxrss(Maxrss) {}

    virtual ~ResourceUsage() = default;

    virtual void provideMapping(json::Output &out);
  };

private:
  // the process identifier of the operating system
  ProcID OSPid;
  // usage information about the process, if available
  llvm::Optional<ResourceUsage> ProcessUsage;

public:
  TaskProcessInfo(ProcID Pid, uint64_t utime, uint64_t stime, uint64_t maxrss)
      : OSPid(Pid), ProcessUsage(ResourceUsage(utime, stime, maxrss)) {}

  TaskProcessInfo(ProcID Pid) : OSPid(Pid), ProcessUsage(None) {}

#if defined(HAVE_GETRUSAGE) && !defined(__HAIKU__)
  TaskProcessInfo(ProcID Pid, struct rusage Usage);
#endif // defined(HAVE_GETRUSAGE) && !defined(__HAIKU__)
  Optional<ResourceUsage> getResourceUsage() { return ProcessUsage; }

  virtual ~TaskProcessInfo() = default;
  virtual void provideMapping(json::Output &out);
};

class TaskQueue {
  friend TaskQueueStats;

protected:
  std::unique_ptr<TaskQueueStats> stats;
  TaskQueueKind kind;
  Context &ctx;

  /// Jobs which have not begun execution.
  std::queue<std::unique_ptr<sys::Task>> runQueue;

  mutable std::mutex taskQueueMutext;
  std::condition_variable taskQueueCondition;

  /// The number of tasks to execute in parallel.
  unsigned parallelTaskCount;

public:
  TaskQueue(TaskQueueKind kind, Context &ctx);

public:
  /// A callback which will be executed when each task begins execution
  ///
  /// \param Pid the ProcessId of the task which just began execution.
  /// \param Context the context which was passed when the task was added
  using TaskBeganCallback = std::function<void(ProcID pid, void *context)>;

  /// A callback which will be executed after each task finishes
  /// execution.
  ///
  /// \param Pid the ProcessId of the task which finished execution.
  /// \param ReturnCode the return code of the task which finished execution.
  /// \param Output the output from the task which finished execution,
  /// if available. (This may not be available on all platforms.)
  /// \param Errors the errors from the task which finished execution, if
  /// available and SeparateErrors was true.  (This may not be available on all
  /// platforms.)
  /// \param ProcInfo contains information like the operating process identifier
  /// and resource usage if available
  /// \param Context the context which was passed when the task was added
  ///
  /// \returns true if further execution of tasks should stop,
  /// false if execution should continue
  using TaskFinishedCallback = std::function<TaskFinishedResponse(
      ProcID pid, int returnCode, llvm::StringRef output,
      llvm::StringRef errors, TaskProcessInfo procInfo, void *context)>;

  /// A callback which will be executed if a task exited abnormally due
  /// to a signal.
  ///
  /// \param Pid the ProcessId of the task which exited abnormally.
  /// \param ErrorMsg a string describing why the task exited abnormally. If
  /// no reason could be deduced, this may be empty.
  /// \param Output the output from the task which exited abnormally, if
  /// available. (This may not be available on all platforms.)
  /// \param Errors the errors from the task which exited abnormally, if
  /// available and SeparateErrors was true.  (This may not be available on all
  /// platforms.)
  /// \param ProcInfo contains information like the operating process identifier
  /// and resource usage if available
  /// \param Context the context which was passed when the task was added
  /// \param Signal the terminating signal number, if available. This may not be
  /// available on all platforms. If it is ever provided, it should not be
  /// removed in future versions of the compiler.
  ///
  /// \returns a TaskFinishedResponse indicating whether or not execution
  /// should proceed
  using TaskSignalledCallback = std::function<TaskFinishedResponse(
      ProcID Pid, StringRef ErrorMsg, StringRef Output, StringRef Errors,
      void *Context, Optional<int> Signal, TaskProcessInfo ProcInfo)>;

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

  /// \returns the maximum number of tasks which this TaskQueue will execute in
  /// parallel
  unsigned GetNumberOfParallelTasks() const;

  /// Adds a task to the TaskQueue.
  ///
  /// \param ExecPath the path to the executable which the task should execute
  /// \param Args the arguments which should be passed to the task
  /// \param Env the environment which should be used for the task;
  /// must be null-terminated. If empty, inherits the parent's environment.
  /// \param Context an optional context which will be associated with the task
  /// \param SeparateErrors Controls whether error output is reported separately
  virtual void AddTask(const char *ExecPath, ArrayRef<const char *> Args,
                       ArrayRef<const char *> Env = llvm::None,
                       void *Context = nullptr, bool SeparateErrors = false);

  /// Synchronously executes the tasks in the TaskQueue.
  ///
  /// \param Began a callback which will be called when a task begins
  /// \param Finished a callback which will be called when a task finishes
  /// \param Signalled a callback which will be called if a task exited
  /// abnormally due to a signal
  ///
  /// \returns true if all tasks did not execute successfully
  virtual bool
  Execute(TaskBeganCallback Began = TaskBeganCallback(),
          TaskFinishedCallback Finished = TaskFinishedCallback(),
          TaskSignalledCallback Signalled = TaskSignalledCallback());

  /// Returns true if there are any tasks that have been queued but have not
  /// yet been executed.
  virtual bool HasRemainingTasks() { return !runQueue.empty(); }
  // ProcID Push(const JobDetail *jobDetail);
  sys::Task *Front();
  void Pop();
  void Print();

  // ProcID CreateTask(const TaskDetail &td);
  //  void Remove(ProcID procID);

public:
  TaskQueueKind GetKind() { return kind; }
  TaskQueueStats &GetStats() { return *stats.get(); }

  int Size() { return runQueue.size(); }
  bool IsEmpty() { return runQueue.empty(); }

public:
  // virtual void AddTask(const TaskDetail &detail);

public:
  void Run(); // TODO: virtual
};
} // namespace stone

// namespace json {
// template <> struct ObjectTraits<stone::TaskProcessInfo> {
//   static void mapping(Output &out, stone::TaskProcessInfo &value) {
//     value.provideMapping(out);
//   }
// };

// template <> struct ObjectTraits<stone::TaskProcessInfo::ResourceUsage> {
//   static void mapping(Output &out,
//                       stone::TaskProcessInfo::ResourceUsage &value) {
//     value.provideMapping(out);
//   }
// };

//} // end namespace json

#endif
