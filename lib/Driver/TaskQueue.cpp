#include "stone/Driver/TaskQueue.h"
#include "stone/Driver/Job.h"

using stone::Job;
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

void stone::TaskQueueStats::Print(ColorfulStream &stream) {}
