#ifndef STONE_CODEGEN_OPTIMIZE_H
#define STONE_CODEGEN_OPTIMIZE_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
} // namespace llvm

namespace stone {
class Pipeline;

/// Parse a source file
void OptimizeIR(llvm::Module *llvmModule);

} // namespace stone
#endif
