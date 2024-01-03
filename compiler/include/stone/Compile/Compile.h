#ifndef STONE_COMPILE_COMPILE_H
#define STONE_COMPILE_COMPILE_H

#include "stone/Basic/Status.h"
#include "stone/IDE.h"

#include "llvm/ADT/ArrayRef.h"

namespace stone {
class Compiler;
class CompilerObservation;

int Compile(llvm::ArrayRef<const char *> args, const char *arg0, void *mainAddr,
            CompilerObservation *observation = nullptr);

/// Handles LLVM
Status CompileLLVMIR(Compiler &compiler);

} // namespace stone

#endif
