#ifndef STONE_COMPILER_DARWINTASKQUEUE_H
#define STONE_COMPILER_DARWINTASKQUEUE_H

#include "stone/Basic/LLVM.h"
#include "stone/Basic/StatisticEngine.h"
#include "stone/Driver/Task.h"
#include "stone/Driver/TaskQueue.h"
#include "stone/Public.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {
namespace darwin {

class DarwinTask final : public sys::Task {
public:
};

class DarwinTaskQueue final : public TaskQueue {
public:
  DarwinTaskQueue(LangContext &ctx) : TaskQueue(TaskQueueKind::Darwin, ctx) {}
};
} // namespace darwin
} // namespace stone
#endif
