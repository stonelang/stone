#ifndef STONE_DRIVER_DRIVER_INSTANCE_H
#define STONE_DRIVER_DRIVER_INSTANCE_H

#include "stone/Driver/DriverOptions.h"

namespace stone {

class DriverInstance final {
  DriverInvocation &invocation;
  DriverObservation *observation = nullptr;

public:
  DriverInstance(const DriverInstance &) = delete;
  void operator=(const Compiler &) = delete;
  DriverInstance(DriverInstance &&) = delete;
  void operator=(DriverInstance &&) = delete;

public:
  DriverInstance(DriverInvocation &invocation);

public:
  DriverInvocation &GetInvocation() { return invocation; }

public:
  bool HasObservation() { return observation != nullptr; }
  void SetObservation(DriverObservation *obs) { observation = obs; }
  DriverObservation *GetObservation() { return observation; }

public:
  Status ExecuteAction(ActionKind kind);
};

} // namespace stone
#endif