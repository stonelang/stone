#ifndef STONE_SUPPORT_MODE_H
#define STONE_SUPPORT_MODE_H

namespace stone {
enum class FrontendMode : unsigned {
#define FRONTEND_MODE(M) M,
#include "stone/Support/Mode.def"
  MAX
};
enum class CompilationMode : unsigned {
#define MODE(M) M,
#include "stone/Support/Mode.def"
  MAX
};

} // namespace stone
#endif
