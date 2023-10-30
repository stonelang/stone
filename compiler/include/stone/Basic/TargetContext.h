#ifndef STONE_BASIC_TARGETCONTEXT_H
#define STONE_BASIC_TARGETCONTEXT_H

#include "llvm/ADT/StringMap.h"
#include "llvm/Support/VersionTuple.h"
#include "llvm/Target/TargetOptions.h"

#include <string>
#include <vector>

namespace stone {
/// Options for controlling the target.
class TargetContext final {
  // TargetOptions targetOpts;
public:
  TargetContext();
};

} // namespace stone

#endif