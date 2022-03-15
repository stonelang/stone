#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class LangListener;

namespace lang {
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            LangListener *listener = nullptr);
}

} // namespace stone
#endif
