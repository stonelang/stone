#ifndef STONE_DRIVER_DRIVER_STEPKIND_H
#define STONE_DRIVER_DRIVER_STEPKIND_H

#include "stone/Basic/Basic.h"

namespace stone {

enum class StepKind : uint8_t {
  Input = 0,
  Compile,
  Backend,
  GeneratePCH,
  MergeModule,
  ModuleWrap,
  Link,
  Interpret,
  AutolinkExtract,
};

}
#endif