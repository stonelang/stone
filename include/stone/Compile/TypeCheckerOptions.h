#ifndef STONE_CHECK_TYPECHECKEROPTIONS_H
#define STONE_CHECK_TYPECHECKEROPTIONS_H

#include <stdint.h>

namespace stone {
namespace types {

enum class TypeCheckMode : uint8_t {
  EachFile = 0,
  WholeModule,
};
class TypeCheckerOptions final {
public:
  TypeCheckMode typeCheckMode = TypeCheckMode::EachFile;
};
} // namespace types
} // namespace stone

#endif
