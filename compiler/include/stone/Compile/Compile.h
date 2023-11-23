#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "stone/Basic/Status.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *listener = nullptr);
}

#endif
