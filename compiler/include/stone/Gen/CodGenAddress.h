#ifndef STONE_GEN_CODEGENADDRESS_H
#define STONE_GEN_CODEGENADDRESS_H

#include "stone/Basic/Mem.h"
#include "stone/Syntax/CodeGenAlignment.h"

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

class CodeGenAddress {
  llvm::Value *addr;
  llvm::Type *elementType;
  CodeGenAlignment alignment;

public:
  CodeGenAddress(llvm::Value *addr, llvm::Type *elementType,
                 CodeGenAlignment alignment)
      : addr(addr), elementType(elementType), alignment(alignment) {}
public:
  llvm::Value *GetAddress() const { return addr; }
  llvm::Type *GetElementType() const { return elementType; }
  CodeGenAlignment GetAlignment() const { return alignment; }
};

} // namespace stone
