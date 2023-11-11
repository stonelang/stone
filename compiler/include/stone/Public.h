#ifndef STONE_PUBLIC_H
#define STONE_PUBLIC_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CompilerListener;
class CompilationListener;

/// Start a compile only session
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);

/// Start a compilation session
int Main(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
         CompilationListener *listener);

} // namespace stone
#endif
