#include "stone/Gen/IRCodeGenTypeResolver.h"
#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Syntax/Type.h"

using namespace stone;

// TODO: Ok for now -- may move to IRCodeGenMoulde
IRCodeGenTypeResolver::IRCodeGenTypeResolver(IRCodeGenModule &cgm) : cgm(cgm) {}

llvm::Type *IRCodeGenTypeResolver::ReloveType(syn::Type ty) { return nullptr; }
