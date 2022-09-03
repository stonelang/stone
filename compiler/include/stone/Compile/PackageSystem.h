#ifndef STONE_COMPILE_PACKAGESYTEM_H
#define STONE_COMPILE_PACKAGESYTEM_H

#include "stone/Compile/CompilerOptions.h"
#include "stone/Syntax/Module.h"
#include "stone/Syntax/SyntaxContext.h"

namespace stone {
class PackageOptions {
public:
};

class PackageSystem final {
  PackageOptions pkgOpts;

public:
  PackageSystem() {}
  ~PackageSystem() {}

public:
  PackageOptions &GetPackageOptions() { return pkgOpts; }
};
} // namespace stone
#endif
