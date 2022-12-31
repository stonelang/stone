#ifndef STONE_GEN_IRCODEGENTYPECACHE_H
#define STONE_GEN_IRCODEGENTYPECACHE_H

namespace llvm {
class Type;
class IntegerType;
class PointerType;
class LLVMContext;
} // namespace llvm

namespace stone {

class IRCodeGenTypeCache {
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

  llvm::IntegerType *CharTy; /// char

  // LLVM Address types
  llvm::IntegerType *RelativeAddressTy;
  llvm::PointerType *RelativeAddressPtrTy;

public:
  IRCodeGenTypeCache(llvm::LLVMContext &llvmContext);
};

} // namespace stone
#endif
