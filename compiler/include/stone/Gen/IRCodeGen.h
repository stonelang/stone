#ifndef STONE_GEN_IRCODEGEN_H
#define STONE_GEN_IRCODEGEN_H

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Gen/IRCodeGenTypeCache.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/PassManager.h"

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class CodeGenListener;

/// A size value, in eight-bit units.
// class CodeGenSize final {
//   private:
//   Int64 val;
// public:
//   constexpr CodeGenSize() : val(0) {}
//   explicit constexpr Size(Int64 val) : val(val) {}

//   static constexpr CodeGenSize forBits(Int64 bitSize) {
//     return CodeGenSize((bitSize + 7U) / 8U);
//   }

//   /// An "invalid" size, equal to the maximum possible size.
//   static constexpr CodeGenSize invalid() { return Size(~Int64(0)); }

//   /// Is this the "invalid" size value?
//   bool IsInvalid() const { return *this == Size::invalid(); }

//   constexpr Int64 getval() const { return val; }

//   Int64 getvalInBits() const { return val * 8; }

//   bool isZero() const { return val == 0; }

//   friend CodeGenSize operator+(CodeGenSize L, CodeGenSize R) {
//     return CodeGenSize(L.val + R.val);
//   }
//   friend CodeGenSize &operator+=(CodeGenSize &L, CodeGenSize R) {
//     L.val += R.val;
//     return L;
//   }

//   friend CodeGenSize operator-(CodeGenSize L, CodeGenSize R) {
//     return CodeGenSize(L.val - R.val);
//   }
//   friend CodeGenSize &operator-=(CodeGenSize &L, CodeGenSize R) {
//     L.val -= R.val;
//     return L;
//   }

//   friend CodeGenSize operator*(CodeGenSize L, Int64 R) {
//     return CodeGenSize(L.val * R);
//   }
//   friend CodeGenSize operator*(Int64 L, CodeGenSize R) {
//     return CodeGenSize(L * R.val);
//   }
//   friend CodeGenSize &operator*=(CodeGenSize &L, Int64 R) {
//     L.val *= R;
//     return L;
//   }

//   friend Int64 operator/(CodeGenSize L, CodeGenSize R) {
//     return L.val / R.val;
//   }

//   explicit operator bool() const { return val != 0; }

//   CodeGenSize RoundUpToAlignment(Alignment align) const {
//     Int64 value = getval() + align.getval() - 1;
//     return CodeGenSize(value & ~Int64(align.getval() - 1));
//   }

//   bool IsPowerOf2() const {
//     auto value = getval();
//     return ((value & -value) == value);
//   }

//   bool IsMultipleOf(CodeGenSize other) const {
//     return (val % other.val) == 0;
//   }

//   unsigned Log2() const {
//     return llvm::Log2_64(val);
//   }

//   operator clang::CharUnits() const {
//     return asCharUnits();
//   }
//   clang::CharUnits AsCharUnits() const {
//     return clang::CharUnits::fromQuantity(getval());
//   }

//   friend bool operator< (CodeGenSize L, CodeGenSize R) { return L.val <
//   R.val; } friend bool operator<=(CodeGenSize L, CodeGenSize R) { return
//   L.val <= R.val; } friend bool operator> (CodeGenSize L, CodeGenSize R) {
//   return L.val >  R.val; } friend bool operator>=(CodeGenSize L, CodeGenSize
//   R) { return L.val >= R.val; } friend bool operator==(CodeGenSize L,
//   CodeGenSize R) { return L.val == R.val; } friend bool
//   operator!=(CodeGenSize L, CodeGenSize R) { return L.val != R.val; }

//   friend CodeGenSize operator%(CodeGenSize L, Alignment R) {
//     return CodeGenSize(L.val % R.getValue());
//   }

// };

class IRCodeGen final {
  CodeGenContext &cgc;
  CodeGenListener *listener;

public:
  IRCodeGen(CodeGenContext &cgc, CodeGenListener *listener = nullptr);
  ~IRCodeGen();
  // IRCodeGenFunction &GetIRCodeGenFunction();

  CodeGenContext &GetCodeGenContext() { return cgc; }
  CodeGenListener *GetCodeGenListener() { return listener; }

public:
  /// Given a global declaration, return a mangled name for this declaration
  /// which has been added to this code generator via a Handle method.
  // llvm::StringRef GetMangledNameForGlobalDecl(Decl *d);

  /// Return the LLVM address of the given global entity.
  ///
  /// \param isForDefinition If true, the caller intends to define the
  ///   entity; the object returned will be an llvm::GlobalValue of
  ///   some sort.  If false, the caller just intends to use the entity;
  ///   the object returned may be any sort of constant value, and the
  ///   code generator will schedule the entity for emission if a
  ///   definition has been registered with this code generator.
  // llvm::Constant *GetAddressForGlobalDecl(Decl *d, bool isForDefinition);
};
} // namespace stone

#endif