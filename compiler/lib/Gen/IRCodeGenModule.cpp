#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRCodeGenABI.h"

#include "stone/Syntax/Decl.h"

using namespace stone;

IRCodeGenModule::IRCodeGenModule(CodeGenContext &codeGenContext,
                                 llvm::StringRef moduleName,
                                 llvm::StringRef outputFilename)

    : codeGenContext(codeGenContext),
      typeCache(*codeGenContext.GetLLVMContext()), moduleName(moduleName),
      outputFilename(outputFilename), typeResolver(*this), metadata(*this) {}

IRCodeGenModule::~IRCodeGenModule() {}

llvm::StringRef IRCodeGenModule::GetMangledName(Decl &d) { return ""; }

// llvm::GlobalValue *IRCodeGenModule::GetGlobalValue(llvm::StringRef name) {
//   return GetCodeGenContext().GetLLVMModule()->getNamedValue(Name);
// }

llvm::Constant *
IRCodeGenModule::CreateFunction(llvm::StringRef mangledName, FunctionDecl *fd,
                                llvm::Type *fnTy,
                                const EmitFunctionOptions emitFunctionOpts,
                                llvm::AttributeList extraAttrs) {

  llvm::GlobalValue *entry; //= GetGlobalValue(MangledName);

  auto isIncompleteFunction = false;
  llvm::FunctionType *llvmFunctionType = nullptr;

  if (llvm::isa<llvm::FunctionType>(fnTy)) {
    llvmFunctionType = llvm::cast<llvm::FunctionType>(fnTy);
  } else {
    llvmFunctionType =
        llvm::FunctionType::get(GetIRCodeGenTypeCache().VoidTy, false);
    isIncompleteFunction = true;
  }

  llvm::Function *llvmFunction =
      llvm::Function::Create(llvmFunctionType, llvm::Function::ExternalLinkage,
                             entry ? llvm::StringRef() : mangledName,
                             GetCodeGenContext().GetLLVMModule());

  return llvmFunction;
}
llvm::Constant *
IRCodeGenModule::GetOrCreateFunction(llvm::StringRef mangledName,
                                     FunctionDecl *fd, llvm::Type *fnTy,
                                     const EmitFunctionOptions emitFunctionOpts,
                                     llvm::AttributeList extraAttrs) {

  // TODO: Ignoring forDefinition for now -- just creating
  return CreateFunction(mangledName, fd, fnTy, emitFunctionOpts, extraAttrs);
}

llvm::Constant *IRCodeGenModule::GetFunctionAddress(
    FunctionDecl *fd, llvm::Type *fnTy,
    const EmitFunctionOptions emitFunctionOpts) {

  // TODO:
  // If there was no specific requested type, just convert it now.
  // if (!functionTy) {
  //   const auto *FD = llvm::cast<FunctionDecl>(GD.getDecl());
  //   functionTy = GetIRCodeGenTypeResolver().ResolveType(fd->GetType());
  // }

  if (emitFunctionOpts.contains(EmitFunctionFlags::IsForDefinition)) {
  }

  return GetOrCreateFunction("mangledName", fd, fnTy, emitFunctionOpts);
}

void IRCodeGenModule::SetFunctionLinkage(FunctionDecl *fd, llvm::Function *fn) {
  fn->setLinkage(GetFunctionLinkage(fd));
}
llvm::GlobalValue::LinkageTypes
IRCodeGenModule::GetFunctionLinkage(FunctionDecl *fd) {

  // TODO: FOR NOW
  return llvm::GlobalValue::InternalLinkage;
}

llvm::StringRef IRCodeGenModule::GetMangledNameOfGlobalDecl(Decl *d) {
  const auto *nd = llvm::cast<NameableDecl>(d);
}
