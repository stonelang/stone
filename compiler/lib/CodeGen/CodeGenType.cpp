#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

llvm::Type *CodeGenModule::GetType(const QualType ty) { return nullptr; }

llvm::FunctionType *CodeGenModule::GetFunctionType(const FunctionDecl *fd) {

  return llvm::FunctionType::get(
      llvm::Type::getVoidTy(GetCodeGenContext().GetLLVMContext()), {}, false);
}
