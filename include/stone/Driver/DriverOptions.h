#ifndef STONE_COMPILE_COMPILEROPTIONS_H
#define STONE_COMPILE_COMPILEROPTIONS_H

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

enum class DriverActionKind : uint8_t {
#define MODE(A) A,
#include "stone/Support/ActionKind.def"
};

class DriverOptions : public Options {
  DriverActionKind primaryActionKind = DriverActionKind::None;

public:
  /// \return the Action
  DriverActionKind GetPrimaryActionKind() const { return primaryActionKind; }
};

} // namespace stone
#endif
