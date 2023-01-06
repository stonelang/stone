#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenABI.h"

#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

IRCodeGenModule::IRCodeGenModule(IRCodeGen &irCodeGen,
                                 llvm::StringRef moduleName,
                                 llvm::StringRef outputFilename)

    : typeCache(*irCodeGen.GetCodeGenContext().GetLLVMContext()),
      irCodeGen(irCodeGen), moduleName(moduleName),
      outputFilename(outputFilename), typeResolver(*this), metadata(*this) {}

IRCodeGenModule::~IRCodeGenModule() {}

llvm::StringRef IRCodeGenModule::GetMangledName(Decl &d) { return ""; }

// llvm::GlobalValue *IRCodeGenModule::GetGlobalValue(llvm::StringRef name) {
//   return GetCodeGenContext().GetLLVMModule()->getNamedValue(Name);
// }

llvm::Constant *IRCodeGenModule::CreateFunction(
    llvm::StringRef mangledName, llvm::Type *functionTy, syn::FunctionDecl *fd,
    EmitFunctionOptions emitFunctionOpts, llvm::AttributeList extraAttrs) {

  llvm::GlobalValue *entry; //= GetGlobalValue(MangledName);

  auto isIncompleteFunction = false;
  llvm::FunctionType *fnTy = nullptr;

  if (llvm::isa<llvm::FunctionType>(functionTy)) {
    fnTy = llvm::cast<llvm::FunctionType>(functionTy);
  } else {
    fnTy = llvm::FunctionType::get(GetIRCodeGenTypeCache().VoidTy, false);
    isIncompleteFunction = true;
  }

  llvm::Function *llvmFunction = llvm::Function::Create(
      fnTy, llvm::Function::ExternalLinkage,
      entry ? llvm::StringRef() : mangledName,
      *GetIRCodeGen().GetCodeGenContext().GetLLVMModule());

  return nullptr;
}
llvm::Constant *IRCodeGenModule::GetOrCreateFunction(
    llvm::StringRef mangledName, llvm::Type *functionTy, syn::FunctionDecl *fd,
    EmitFunctionOptions emitFunctionOpts, llvm::AttributeList extraAttrs) {

  // TODO: Ignoring forDefinition for now -- just creating
  return CreateFunction(mangledName, functionTy, fd, emitFunctionOpts,
                        extraAttrs);
}

llvm::Constant *
IRCodeGenModule::GetFunctionAddress(syn::FunctionDecl *fd,
                                    llvm::Type *functionTy,
                                    EmitFunctionOptions emitFunctionOpts) {

  // TODO:
  // If there was no specific requested type, just convert it now.
  // if (!functionTy) {
  //   const auto *FD = llvm::cast<FunctionDecl>(GD.getDecl());
  //   functionTy = GetIRCodeGenTypeResolver().ResolveType(fd->GetType());
  // }

  return nullptr;
}
