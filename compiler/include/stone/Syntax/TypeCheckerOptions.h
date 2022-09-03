#ifndef STONE_SYNTAX_TYPECHECKEROPTIONS_H
#define STONE_SYNTAX_TYPECHECKEROPTIONS_H

#include "stone/Basic/STDTypeAlias.h"

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
