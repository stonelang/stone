
#include "stone/AST/Decl.h"
#include "stone/AST/Global.h"
#include "stone/Gen/IRGenFunction.h"
#include "stone/Gen/IRGenModule.h"

using namespace stone;

void IRGenModule::EmitFunDecl(FunDecl *funDecl,
                              llvm::GlobalValue *globalValue) {

  assert(funDecl && "Null FundDecl");

  auto funDeclType = GetIRGenTypeResolver().GetFunctionType(funDecl);

  EmitFunctionOptions emitFunctionOpts;
  emitFunctionOpts |= EmitFunctionFlags::IsForDefinition;
  emitFunctionOpts |= EmitFunctionFlags::DontDefer;

  if (!globalValue) {
    globalValue = llvm::cast<llvm::GlobalValue>(
        GetFunctionAddress(funDecl, funDeclType, emitFunctionOpts));
  }

  auto *llvmFunction = cast<llvm::Function>(globalValue);

  SetFunctionLinkage(funDecl, llvmFunction);
  IRGenFunction(*this, llvmFunction).EmitFunction(funDecl);
}
