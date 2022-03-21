#ifndef STONE_COMPILER_JOBQUEUE_H
#define STONE_COMPILER_JOBQUEUE_H

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

class Job;
class JobQueue;
class JobQueueStats final : public Stats {
  const JobQueue &queue;

public:
  JobQueueStats(const JobQueue &queue, Context &ctx)
      : Stats("task-queue statistics:", ctx), queue(queue) {}
  void Print() override;
};

using ProcID = llvm::sys::procid_t;
enum class JobQueueKind { None, Darwin, Unix, Win };

class JobQueue {
  friend JobQueueStats;

protected:
  std::unique_ptr<JobQueueStats> stats;
  JobQueueKind kind;
  Context &ctx;

  /// Jobs which have not begun execution.
  std::queue<Job *> entries;

  mutable std::mutex jobQueueMutext;
  std::condition_variable jobQueuCondition;

  /// The number of tasks to execute in parallel.
  unsigned parallelJobCount;

public:
  JobQueue(JobQueueKind kind, Context &ctx);

public:
  ProcID Push(Job *job);
  // Job *Dequeue(ProcID procID);
  Job *Front();
  void Pop();
  void Print();

  // void Remove(ProcID procID);

public:
  JobQueueKind GetKind() { return kind; }
  JobQueueStats &GetStats() { return *stats.get(); }

  int Size() { return entries.size(); }
  bool IsEmpty() { return entries.empty(); }
};

class DarwinJobQueue final : public JobQueue {
public:
  DarwinJobQueue(Context &ctx) : JobQueue(JobQueueKind::Darwin, ctx) {}
};

class UnixJobQueue final : public JobQueue {
public:
  UnixJobQueue(Context &ctx) : JobQueue(JobQueueKind::Unix, ctx) {}
};

class WinJobQueue final : public JobQueue {
public:
  WinJobQueue(Context &ctx) : JobQueue(JobQueueKind::Win, ctx) {}
};
} // namespace stone
#endif
