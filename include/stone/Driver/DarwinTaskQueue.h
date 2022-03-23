#ifndef STONE_COMPILER_DARWINTASKQUEUE_H
#define STONE_COMPILER_DARWINTASKQUEUE_H

#include <functional>
#include <memory>
#include <queue>

#include "stone/Core/Context.h"
#include "stone/Core/LLVM.h"
#include "stone/Core/StatisticEngine.h"
#include "stone/Driver/TaskQueue.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/Config/config.h"
#include "llvm/Support/Program.h"

namespace stone {
namespace darwin {
class DarwinTaskQueue final : public TaskQueue {
public:
  DarwinTaskQueue(Context &ctx) : TaskQueue(TaskQueueKind::Darwin, ctx) {}
};
} // namespace darwin
} // namespace stone
#endif
