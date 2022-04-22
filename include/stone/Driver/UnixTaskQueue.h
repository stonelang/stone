#ifndef STONE_COMPILER_UNIXTASKQUEUE_H
#define STONE_COMPILER_UNIXTASKQUEUE_H

#include "stone/Basic/Context.h"
#include "stone/Basic/LLVM.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/Task.h"
#include "stone/Driver/TaskQueue.h"

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {
namespace unix {

class UnixTask final : public sys::Task {
public:
};

class UnixTaskQueue final : public TaskQueue {
public:
  UnixTaskQueue(Context &ctx) : TaskQueue(TaskQueueKind::Unix, ctx) {}
};
} // namespace unix
} // namespace stone
#endif
