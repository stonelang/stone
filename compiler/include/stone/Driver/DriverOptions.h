#ifndef STONE_DRIVER_DRIVERNOPTIONS_H
#define STONE_DRIVER_DRIVERNOPTIONS_H

#include "stone/Option/Action.h"
#include "stone/Option/Options.h"

#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"

using namespace llvm::opt;

namespace stone {

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

public:
  /// The main action requested or computed.
  Action mainAction;

public:
  /// Default target triple.
  std::string defaultTargetTriple;

  /// The path the executing program
  llvm::StringRef mainExecutablePath;

  /// The name of the executing program
  llvm::StringRef mainExecutableName;
};

} // namespace stone

#endif