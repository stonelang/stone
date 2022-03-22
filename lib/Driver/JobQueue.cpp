#include "stone/Driver/JobQueue.h"

#include "stone/Driver/Job.h"

using stone::Job;
using stone::JobQueue;

JobQueue::JobQueue(JobQueueKind kind, Context &ctx) : kind(kind), ctx(ctx) {}

stone::ProcID JobQueue::Push(Job *job) {
  runQueue.push(job);
  job->SetID(runQueue.size());
}

// Job *JobQueue::Dequeue(stone::ProcID procID) { return nullptr; }

Job *JobQueue::Front() { return runQueue.front(); }
void JobQueue::Pop() { runQueue.pop(); }

// void JobQueue::Remove(stone::ProcID procID) {}
