#ifndef STONE_DRIVER_MAIN_H
#define STONE_DRIVER_MAIN_H

#include "stone/Basic/Status.h"
#include "stone/IDE.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class DriverInstance;
class DriverInvocation;

class DriverObservation {
public:
  DriverObservation() = default;
  virtual ~DriverObservation() = default;

public:
  /// The command line has been parsed.
  virtual void CompletedCommandLineParsing(DriverInvocation &invocation);

  /// The compiler has been configured
  virtual void CompletedConfiguration(DriverInstance &instance);
};

int Main(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
         DriverObservation *observation = nullptr);

} // namespace stone

#endif
