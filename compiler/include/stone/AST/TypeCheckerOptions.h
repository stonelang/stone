#ifndef STONE_ASTTYPECHECKEROPTIONS_H
#define STONE_ASTTYPECHECKEROPTIONS_H

#include "stone/Basic/STDAlias.h"

namespace stone {

enum class TypeCheckMode : UInt8 {
  EachFile = 0,
  WholeModule,
};
class TypeCheckerOptions final {
public:
  TypeCheckMode typeCheckMode = TypeCheckMode::EachFile;
};
} // namespace stone

#endif
