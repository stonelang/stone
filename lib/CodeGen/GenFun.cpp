#include "stone/AST/Decl.h"
#include "stone/AST/Global.h"
#include "stone/AST/Module.h"
#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

// void CodeGenModule::EmitFunDecl(FunDecl *FD) {

//   assert(FD && "Null FundDecl");

//   // EmitFunctionDecl(FD);

//   // if (FD->HasBody()) {
//   // }

//   // auto funType = GetFunctionType(funDecl);
// }

void CodeGenModule::EmitFunDecl(FunDecl *FD, llvm::GlobalValue *GV) {

  assert(FD && "Null FundDecl!");

  auto funType = GetFunctionType(FD);

  //   EmitFunctionOptions emitFunctionOpts;
  //   emitFunctionOpts |= EmitFunctionFlags::IsForDefinition;
  //   emitFunctionOpts |= EmitFunctionFlags::DontDefer;

  //   if (!globalValue) {
  //     globalValue = llvm::cast<llvm::GlobalValue>(
  //         GetFunctionAddress(funDecl, funDeclType, emitFunctionOpts));
  //   }

  //   auto *llvmFunction = cast<llvm::Function>(globalValue);

  //   SetFunctionLinkage(funDecl, llvmFunction);
  //   CodeGenFunction(*this, llvmFunction).EmitFunction(funDecl);
}
