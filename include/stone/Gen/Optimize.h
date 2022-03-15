#ifndef STONE_GEN_OPTIMIZE_H
#define STONE_GEN_OPTIMIZE_H

#include "llvm/ADT/ArrayRef.h"

namespace llvm {
class Module;
} // namespace llvm

namespace stone {
/// Parse a source file
/// Think about passing IRModule
void OptimizeIR(llvm::Module *llvmMod);

} // namespace stone
#endif
