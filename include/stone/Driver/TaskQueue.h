#ifndef STONE_COMPILER_TASKQUEUE_H
#define STONE_COMPILER_TASKQUEUE_H

#include <functional>
#include <memory>
#include <queue>

#include "stone/Basic/Context.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/Task.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {

class Task;
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
  // ProcID Push(const JobDetail *jobDetail);
  sys::Task *Front();
  void Pop();
  void Print();

  //ProcID CreateTask(const TaskDetail &td);
  // void Remove(ProcID procID);

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
#endif
