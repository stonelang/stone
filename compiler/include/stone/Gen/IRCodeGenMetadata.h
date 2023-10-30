#ifndef STONE_GEN_IRCODEGENMETADATA_H
#define STONE_GEN_IRCODEGENMETADATA_H

#include "clang/AST/CharUnits.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/MathExtras.h"

#include "llvm/ADT/ilist.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
class IRCodeGenModule;
class IRCodeGenMetadata final {
  IRCodeGenModule &cgm;

public:
  IRCodeGenMetadata(IRCodeGenModule &cgm);
};

} // namespace stone

#endif
