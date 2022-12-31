#include "stone/Gen/IRCodeGenTypeResolver.h"
#include "stone/Gen/IRCodeGenABI.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Type.h"

using namespace stone;

// TODO: Ok for now -- may move to IRCodeGenMoulde
IRCodeGenTypeResolver::IRCodeGenTypeResolver(IRCodeGenModule &cgm) : cgm(cgm) {}

llvm::Type *IRCodeGenTypeResolver::ResolveType(syn::Type ty) { return nullptr; }

llvm::Type *
IRCodeGenABI::ResolveMemberPointerType(const syn::MemberPointerType *mpt) {
  // return cgm.GetIRCodeGenTypeResolver().ResolveType(mpt);
  return nullptr;
}