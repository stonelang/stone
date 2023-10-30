#ifndef STONE_COMPILER_UNIXTASKQUEUE_H
#define STONE_COMPILER_UNIXTASKQUEUE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Drive/Task.h"
#include "stone/Drive/TaskQueue.h"
#include "stone/Public.h"

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
  UnixTaskQueue(LangContext &ctx) : TaskQueue(TaskQueueKind::Unix, ctx) {}
};
} // namespace unix
} // namespace stone
#endif
