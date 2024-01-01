#ifndef STONE_DRIVER_DRIVER_OPTIONS_H
#define STONE_DRIVER_DRIVER_OPTIONS_H

#include "stone/Basic/CodeGenOptions.h"
#include "stone/Basic/LangOptions.h"
#include "stone/Basic/STDAlias.h"
#include "stone/Basic/Status.h"
#include "stone/Driver/DriverInputFile.h"
#include "stone/Option/Options.h"

#include "llvm/ADT/SetVector.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Triple.h"

namespace stone {

enum class LinkMode : UInt8 {
  // We are not linking
  None = 0,
  // The default output compiling -- sc looks afor a main file and
  // outputs an executable file
  Executable,

  // The default library output: 'stone test.stone -emit-library -> test.dylib'
  DynamicLibrary,

  // The Library output that requires static: 'stone test.stone -emit-library
  // -satic -> test.a'
  StaticLibrary
};

class DriverOptions final {

public:
  String defaultTarget;
};

} // namespace stone

#endif