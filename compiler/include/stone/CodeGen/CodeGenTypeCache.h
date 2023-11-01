#ifndef STONE_GEN_CODEGENTYPECACHE_H
#define STONE_GEN_CODEGENTYPECACHE_H

namespace llvm {
class Type;
class IntegerType;
class PointerType;
class LLVMContext;
} // namespace llvm

namespace stone {
namespace codegen {
class CodeGenTypeCache {
public:
  llvm::Type *VoidTy;

  llvm::IntegerType *Int8Ty;  /// i8
  llvm::IntegerType *Int16Ty; /// i16
  llvm::IntegerType *Int32Ty; /// i32
  llvm::IntegerType *Int64Ty; /// i64
  llvm::IntegerType *IntTy;   /// int

  llvm::PointerType *Int8PtrTy;  ///  i8*
  llvm::PointerType *Int16PtrTy; /// i16*
  llvm::PointerType *Int32PtrTy; /// i32*
  llvm::PointerType *Int64PtrTy; /// i64*

public:
  llvm::IntegerType *CharTy; /// char

  // LLVM Address types
  llvm::IntegerType *RelativeAddressTy;
  llvm::PointerType *RelativeAddressPtrTy;

public:
  explicit CodeGenTypeCache(llvm::LLVMContext &llvmContext);
};
} // namespace codegen
} // namespace stone
#endif
