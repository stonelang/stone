#include "stone/AST/Type.h"
#include "stone/CodeGen/CodeGenTypeResolver.h"
#include "stone/CodeGen/CodeGenABI.h"
#include "stone/CodeGen/CodeGenModule.h"

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
using namespace stone::codegen;

// TODO: Ok for now -- may move to CodeGenMoulde
CodeGenTypeResolver::CodeGenTypeResolver(CodeGenModule &cgm) : cgm(cgm) {}

llvm::Type *CodeGenTypeResolver::GetType(const ast::Type ty) { return nullptr; }

llvm::FunctionType *
CodeGenTypeResolver::GetFunctionType(const ast::FunctionDecl *fd) {

  // llvm::SmallVector<llvm::Type*, 8> argTypes(IRFunctionArgs.totalIRArgs());

  // TODO: Just for now
  return llvm::FunctionType::get(
      llvm::Type::getVoidTy(cgm.GetCodeGenContext().GetLLVMContext()), {},
      false);
}

// llvm::Type *CodeGenTypeResolver::GetType(const ast::MemberPointerType mpty)
// {
//   return nullptr;
// }

// llvm::FunctionType *CodeGenTypeResolver::GetFunctionType(const
// ast::FunctionType fty) {
//   return nullptr;
// }
