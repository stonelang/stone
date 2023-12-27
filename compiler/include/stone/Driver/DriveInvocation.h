#ifndef STONE_DRIVER_DRIVER_INVOCATION_H
#define STONE_DRIVER_DRIVER_INVOCATION_H

namespace stone {

class DriverInvocation final {
public:
  DriverInvocation();

public:
  Status ParseCommandLine(llvm::ArrayRef<const char *> args);
};

} // namespace stone
#endif
