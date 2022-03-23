#ifndef STONE_COMPILER_TASKQUEUE_H
#define STONE_COMPILER_TASKQUEUE_H

#include <functional>
#include <memory>
#include <queue>

#include "stone/Core/Context.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/Job.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {

/// TODO:
class Task {};
class TaskDetail;

class TaskQueue;
class TaskQueueStats final : public Stats {
  const TaskQueue &queue;

public:
  TaskQueueStats(const TaskQueue &queue, Context &ctx)
      : Stats("task-queue statistics:", ctx), queue(queue) {}
  void Print() override;
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
  std::queue<std::unique_ptr<Task>> runQueue;

  mutable std::mutex taskQueueMutext;
  std::condition_variable taskQueueCondition;

  /// The number of tasks to execute in parallel.
  unsigned parallelJobCount;

public:
  TaskQueue(TaskQueueKind kind, Context &ctx);

public:
  ProcID Push(const TaskDetail *taskDetail);
  Job *Front();
  void Pop();
  void Print();

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
