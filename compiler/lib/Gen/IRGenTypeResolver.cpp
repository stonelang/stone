#include "stone/Gen/IRGenTypeResolver.h"
#include "stone/AST/Type.h"
#include "stone/Gen/IRGenABI.h"
#include "stone/Gen/IRGenInvocation.h"

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

// TODO: Ok for now -- may move to IRGenMoulde
IRGenTypeResolver::IRGenTypeResolver(IRGenModule &cgm) : cgm(cgm) {}

llvm::Type *IRGenTypeResolver::GetType(const Type ty) { return nullptr; }

llvm::FunctionType *IRGenTypeResolver::GetFunctionType(const FunctionDecl *fd) {

  // llvm::SmallVector<llvm::Type*, 8> argTypes(IRFunctionArgs.totalIRArgs());

  // TODO: Just for now
  return llvm::FunctionType::get(
      llvm::Type::getVoidTy(cgm.GetInvocation().GetLLVMContext()), {}, false);
}

// llvm::Type *IRGenTypeResolver::GetType(const MemberPointerType mpty)
// {
//   return nullptr;
// }

// llvm::FunctionType *IRGenTypeResolver::GetFunctionType(const
// FunctionType fty) {
//   return nullptr;
// }
