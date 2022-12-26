#include "stone/Gen/IRCodeGen.h"
#include "stone/Basic/CodeGenOptions.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

using namespace stone;

// TODO: Ok for now -- may move to IRCodeGenMoulde
IRCodeGenTypeCache::IRCodeGenTypeCache(llvm::LLVMContext &llvmContext) {

  VoidTy = llvm::Type::getVoidTy(llvmContext);

  // Int8Ty = llvm::Type::getInt8Ty(getLLVMContext());
  // Int16Ty = llvm::Type::getInt16Ty(getLLVMContext());
  // Int32Ty = llvm::Type::getInt32Ty(getLLVMContext());
  // Int32PtrTy = Int32Ty->getPointerTo();
  // Int64Ty = llvm::Type::getInt64Ty(getLLVMContext());
  // Int8PtrTy = llvm::Type::getInt8PtrTy(getLLVMContext());
  // Int8PtrPtrTy = Int8PtrTy->getPointerTo(0);
}

IRCodeGen::IRCodeGen(CodeGenContext &cgc, CodeGenListener *listener)
    : cgc(cgc), listener(listener), typeCache(cgc.GetLLVMContext()),
      cgb(cgc, typeCache) {}

IRCodeGen::~IRCodeGen() {}

Safe<llvm::TargetMachine> IRCodeGen::CreateTargetMachine() {}