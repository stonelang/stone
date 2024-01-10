#include "stone/Driver/TaskQueue.h"
#include "stone/Stats/Stats.h"

using namespace stone;

// Include the correct TaskQueue implementation.
#if LLVM_ON_UNIX && !defined(__CYGWIN__) && !defined(__HAIKU__)
#include "UnixTaskQueue.inc"
#elif defined(_WIN32)
#include "WindowsTaskQueue.inc"
#else
#include "DefaultTaskQueue.inc"
#endif

TaskQueue::TaskQueue(unsigned numberOfParallelTasks, DriverStatsReporter *stats)
    : numberOfParallelTasks(numberOfParallelTasks), stats(stats) {}
