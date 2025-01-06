#include "stone/AST/Decl.h"
#include "stone/AST/Global.h"
#include "stone/AST/Type.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

llvm::Type *CodeGenModule::GetType(const Type QT) { return nullptr; }

llvm::FunctionType *CodeGenModule::GetFunctionType(const FunctionDecl *FD) {

  return llvm::FunctionType::get(
      llvm::Type::getVoidTy(GetCodeGenContext().GetLLVMContext()), {}, false);
}
