#ifndef STONE_GEN_CODEGENALIGNMENT_H
#define STONE_GEN_CODEGENALIGNMENT_H

#include "stone/Basic/Mem.h"
#include "stone/Basic/STDAlias.h"

#include "clang/AST/CharUnits.h"
#include "clang/CodeGen/ConstantInitFuture.h"
#include "llvm/ADT/ilist.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/DerivedType.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/DataType.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {
namespace codegen {

/// An alignment value, in eight-bit units.
class CodGenAlignment {
  unsigned char shift;

public:
  constexpr CodeGenAlignment() : shift(0) {}

  explicit CodeGenAlignment(UInt64 Value) : shift(llvm::Log2_64(Value)) {
    assert(llvm::isPowerOf2_64(Value));
  }
  explicit CodeGenAlignment(clang::CharUnits value)
      : CodeGenAlignment(value.getQuantity()) {}

  constexpr UInt64 GetValue() const { return UInt64(1) << shift; }
  constexpr UInt64 GetMaskValue() const { return getValue() - 1; }

  CodeGenAlignment AlignmentAtOffset(Size S) const;
  Size AsSize() const;

  unsigned Log2() const { return shift; }

  operator clang::CharUnits() const { return AsCharUnits(); }

  clang::CharUnits AsCharUnits() const {
    return clang::CharUnits::fromQuantity(getValue());
  }

  explicit operator llvm::MaybeAlign() const {
    return llvm::MaybeAlign(getValue());
  }

  friend bool operator<(CodeGenAlignment L, CodeGenAlignment R) {
    return L.shift < R.shift;
  }
  friend bool operator<=(CodeGenAlignment L, CodeGenAlignment R) {
    return L.shift <= R.shift;
  }
  friend bool operator>(CodeGenAlignment L, CodeGenAlignment R) {
    return L.shift > R.shift;
  }
  friend bool operator>=(CodeGenAlignment L, CodeGenAlignment R) {
    return L.shift >= R.shift;
  }
  friend bool operator==(CodeGenAlignment L, CodeGenAlignment R) {
    return L.shift == R.shift;
  }
  friend bool operator!=(CodeGenAlignment L, CodeGenAlignment R) {
    return L.shift != R.shift;
  }

  template <unsigned Value> static constexpr CodeGenAlignment Create() {
    CodeGenAlignment result;
    result.shift = llvm::CTLog2<Value>();
    return result;
  }
};
} // namespace codegen
} // namespace stone
