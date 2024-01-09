#include "stone/Driver/TaskQueue.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"

#include "Task.inc"

using namespace llvm::sys;

bool TaskQueue::SupportsBufferingOutput() {
  // The default implementation supports buffering output.
  return true;
}

bool TaskQueue::SupportsParallelExecution() {
  // The default implementation does not support parallel execution.
  return false;
}

unsigned TaskQueue::GetNumberOfParallelTasks() const {
  // The default implementation does not support parallel execution.
  return 1;
}
