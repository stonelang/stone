#ifndef STONE_COMPILE_MODULEDEPENDENCIES_H
#define STONE_COMPILE_MODULEDEPENDENCIES_H

#include "stone/Compile/CompilerOptions.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"

namespace stone {

/// The kind of module dependency we are looking for.
enum class ModuleDependenciesKind : int8_t {
  First = 0,
  Interface = First,
  Binary,
  Placeholder,
  Clang,
  Source,
  Last = Source + 1
};
class ModuleDependencies {
public:
};
} // namespace stone
#endif
