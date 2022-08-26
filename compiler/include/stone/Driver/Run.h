#ifndef STONE_DRIVER_RUN_H
#define STONE_DRIVER_RUN_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CompilationListener;

int Run(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
        CompilationListener *listener);
} // namespace stone
#endif
