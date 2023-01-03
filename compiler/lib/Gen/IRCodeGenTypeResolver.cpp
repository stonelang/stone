#include "stone/Gen/IRCodeGenTypeResolver.h"
#include "stone/Gen/IRCodeGenABI.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Type.h"

using namespace stone;

// TODO: Ok for now -- may move to IRCodeGenMoulde
IRCodeGenTypeResolver::IRCodeGenTypeResolver(IRCodeGenModule &cgm) : cgm(cgm) {}

llvm::Type *IRCodeGenTypeResolver::GetType(const syn::Type ty) {
  return nullptr;
}

// llvm::Type *IRCodeGenTypeResolver::GetType(const syn::MemberPointerType mpty)
// {
//   return nullptr;
// }

// llvm::FunctionType *IRCodeGenTypeResolver::GetFunctionType(const
// syn::FunctionType fty) {
//   return nullptr;
// }
