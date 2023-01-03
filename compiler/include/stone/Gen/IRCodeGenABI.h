#ifndef STONE_GEN_IRCODEGENABI_H
#define STONE_GEN_IRCODEGENABI_H

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
namespace syn {
class MemberPointerType;
}
class IRCodeGenModule;

class IRCodeGenABI final {
  IRCodeGenModule &cgm;

public:
  IRCodeGenABI(IRCodeGenModule &cgm);

public:
  llvm::Type *ResolveMemberPointerType(const syn::MemberPointerType mpt);
};

} // namespace stone

#endif
