#ifndef STONE_GEN_CODEGENABI_H
#define STONE_GEN_CODEGENABI_H

#include "stone/AST/Mangle.h"

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

class MemberPointerType;
class CodeGenModule;

class CodeGenABI final {
  CodeGenModule &cgm;

  std::unique_ptr<MangleContext> mangleContext;

public:
  CodeGenABI(CodeGenModule &cgm);

public:
  llvm::Type *ResolveMemberPointerType(const MemberPointerType mpt);
};
} // namespace stone

#endif
