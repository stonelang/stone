#include "stone/Driver/TaskQueue.h"

using stone::TaskQueue;

TaskQueue::TaskQueue(TaskQueueKind kind, Context &ctx) : kind(kind), ctx(ctx) {}

// stone::ProcID TaskQueue::Push(TaskDetail *job) {
//   //runQueue.push(job);
//   job->SetID(runQueue.size());
// }

// // Job *TaskQueue::Dequeue(stone::ProcID procID) { return nullptr; }

// Job *TaskQueue::Front() { return runQueue.front(); }
// void TaskQueue::Pop() { runQueue.pop(); }

// void TaskQueue::Remove(stone::ProcID procID) {}

void TaskQueue::AddTask(const char *ExecPath, ArrayRef<const char *> Args,
                        ArrayRef<const char *> Env, void *Context,
                        bool SeparateErrors) {}

/// Synchronously executes the tasks in the TaskQueue.
///
/// \param Began a callback which will be called when a task begins
/// \param Finished a callback which will be called when a task finishes
/// \param Signalled a callback which will be called if a task exited
/// abnormally due to a signal
///
/// \returns true if all tasks did not execute successfully
bool TaskQueue::Execute(TaskBeganCallback Began,
                                TaskFinishedCallback Finished,
                                TaskSignalledCallback Signalled) {

  return true;
}

void stone::TaskQueueStats::Print(ColorfulStream &stream) {}
