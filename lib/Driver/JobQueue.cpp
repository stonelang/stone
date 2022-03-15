#include "stone/Driver/JobQueue.h"
#include "stone/Driver/Job.h"

using stone::Job;
using stone::JobQueue;

JobQueue::JobQueue(JobQueueKind kind, Context &ctx) : kind(kind), ctx(ctx) {}

stone::ProcID JobQueue::Push(Job *job) {
  entries.push(job);
  job->queueID = entries.size();
}

// Job *JobQueue::Dequeue(stone::ProcID procID) { return nullptr; }

Job *JobQueue::Front() { return entries.front(); }
void JobQueue::Pop() { entries.pop(); }

// void JobQueue::Remove(stone::ProcID procID) {}
