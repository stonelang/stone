#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "stone/Basic/File.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Compiler;
class InFlightMode;
class CompilerListener;

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerListener *cl = nullptr);

int Compile(Compiler &compiler, file::File &input);

} // namespace stone
#endif
