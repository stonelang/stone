#ifndef STONE_DRIVER_DRIVEROPTIONS_H
#define STONE_DRIVER_DRIVEROPTIONS_H

#include "stone/Driver/StepKind.h"
#include "stone/Support/Options.h"

#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Option/Arg.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"

#include <stdint.h>
#include <string>

namespace stone {

enum class LinkType : uint8_t {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  Executable,

  // The default library output: 'stone test.stone -emit-library ->test.dylib'
  DynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  StaticLibrary
};

enum class DriverActionKind : uint8_t {
#define MODE(A) A,
#include "stone/Support/ActionKind.def"
};

class DriverOptions : public Options {
  DriverActionKind primaryActionKind = DriverActionKind::None;

public:
  /// \return the Action
  DriverActionKind GetPrimaryActionKind() const { return primaryActionKind; }

public:
  static llvm::ArrayRef<StepKind> GetStepKindList(DriverActionKind kind);
};

} // namespace stone
#endif
