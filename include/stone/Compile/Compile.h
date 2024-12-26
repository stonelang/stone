#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/Module.h"

namespace stone {

class CompilerObservation;
int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerObservation *observation = nullptr);

} // namespace stone

#endif
