#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Basic/LangOptions.h"
#include "stone/Option/Action.h"

namespace stone {
class DriverInstance;
class DriverInvocation;

enum class LinkMode : uint8_t {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  EmitExecutable,

  // The default library output: 'stone test.stone -emit-library -> test.dylib'
  EmitDynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  EmitStaticLibrary
};

class DriverOptions final {
  friend DriverInstance;
  friend DriverInvocation;

public:
  /// The main action requested or computed.
  Action mainAction;
};

} // namespace stone

#endif