#ifndef STONE_DRIVER_DRIVE_H
#define STONE_DRIVER_DRIVE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class CompilationObserver;

int Main(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
         CompilationObserver *observer);
} // namespace stone
#endif
