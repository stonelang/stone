#include "stone/Gen/CodeGenTypeResolver.h"
#include "stone/Gen/CodeGen.h"
#include "stone/Gen/CodeGenABI.h"
#include "stone/Gen/CodeGenModule.h"
#include "stone/Syntax/Type.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Assumptions.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/Utils/Local.h"

using namespace stone;

// TODO: Ok for now -- may move to CodeGenMoulde
CodeGenTypeResolver::CodeGenTypeResolver(CodeGenModule &cgm) : cgm(cgm) {}

llvm::Type *CodeGenTypeResolver::GetType(const syn::Type ty) {
  return nullptr;
}

llvm::FunctionType *
CodeGenTypeResolver::GetFunctionType(const syn::FunctionDecl *fd) {

  // llvm::SmallVector<llvm::Type*, 8> argTypes(IRFunctionArgs.totalIRArgs());

  // TODO: Just for now
  return llvm::FunctionType::get(
      llvm::Type::getVoidTy(
          *cgm.GetCodeGen().GetCodeGenContext().GetLLVMContext()),
      {}, false);
}

// llvm::Type *CodeGenTypeResolver::GetType(const syn::MemberPointerType mpty)
// {
//   return nullptr;
// }

// llvm::FunctionType *CodeGenTypeResolver::GetFunctionType(const
// syn::FunctionType fty) {
//   return nullptr;
// }
