#ifndef STONE_DRIVER_DRIVER_INSTANCE_H
#define STONE_DRIVER_DRIVER_INSTANCE_H

namespace stone {

class DriverInstance final {
  DriverInvocation &invocation;

public:
  DriverInstance(DriverInvocation &invocation);

public:
  DriverInvocation &GetInvocation() { return invocation; }

public:
  Status ExecuteAction(ActionKind kind);
};

} // namespace stone
#endif
