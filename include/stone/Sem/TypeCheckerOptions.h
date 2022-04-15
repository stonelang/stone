#ifndef STONE_SEM_TYPECHECKEROPTIONS_H
#define STONE_SEM_TYPECHECKEROPTIONS_H

#include <stdint.h>

namespace stone {
namespace sem {

enum class TypeCheckMode : uint8_t {
  EachFile = 0,
  WholeModule,
};
class TypeCheckerOptions final {
public:
  TypeCheckMode typeCheckMode = TypeCheckMode::EachFile;
};
} // namespace sem
} // namespace stone

#endif
