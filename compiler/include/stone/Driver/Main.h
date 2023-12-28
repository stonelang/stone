#ifndef STONE_DRIVER_MAIN_H
#define STONE_DRIVER_MAIN_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
int Main(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr);

} // namespace stone

#endif
