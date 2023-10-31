#include "stone/AST/Module.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/CodeGen/CodeGen.h"
#include "stone/Lang.h"

#include "llvm/IR/Module.h"

using namespace stone;

// TODO: Ok for now -- may move to CodeGenMoulde
CodeGenTypeCache::CodeGenTypeCache(llvm::LLVMContext &llvmContext) {

  VoidTy = llvm::Type::getVoidTy(llvmContext);
  Int8Ty = llvm::Type::getInt8Ty(llvmContext);
  Int16Ty = llvm::Type::getInt16Ty(llvmContext);
  Int32Ty = llvm::Type::getInt32Ty(llvmContext);
  Int32PtrTy = Int32Ty->getPointerTo();
  Int64Ty = llvm::Type::getInt64Ty(llvmContext);
  Int8PtrTy = llvm::Type::getInt8PtrTy(llvmContext);

  // Int8PtrPtrTy = Int8PtrTy->getPointerTo(0);
}

CodeGen::CodeGen(CodeGenContext &cgc, CodeGenListener *listener)
    : cgc(cgc), listener(listener) {}

CodeGen::~CodeGen() {}
