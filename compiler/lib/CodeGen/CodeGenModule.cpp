#include "stone/CodeGen/CodeGenModule.h"
#include "stone/CodeGen/CodeGenABI.h"

#include "stone/AST/Decl.h"

using namespace stone;

CodeGenModule::CodeGenModule(CodeGenContext &cgc, llvm::StringRef moduleName,
                             llvm::StringRef outputFilename)

    : typeCache(cgc.GetLLVMContext()), cgc(cgc), moduleName(moduleName),
      outputFilename(outputFilename), typeResolver(*this), metadata(*this) {}

CodeGenModule::~CodeGenModule() {}

llvm::StringRef CodeGenModule::GetMangledName(Decl &d) { return ""; }

// llvm::GlobalValue *CodeGenModule::GetGlobalValue(llvm::StringRef name) {
//   return GetCodeGenContext().GetLLVMModule()->getNamedValue(Name);
// }

llvm::Constant *
CodeGenModule::CreateFunction(llvm::StringRef mangledName,
                              stone::FunctionDecl *fd, llvm::Type *fnTy,
                              const EmitFunctionOptions emitFunctionOpts,
                              llvm::AttributeList extraAttrs) {

  llvm::GlobalValue *entry; //= GetGlobalValue(MangledName);

  auto isIncompleteFunction = false;
  llvm::FunctionType *llvmFunctionType = nullptr;

  if (llvm::isa<llvm::FunctionType>(fnTy)) {
    llvmFunctionType = llvm::cast<llvm::FunctionType>(fnTy);
  } else {
    llvmFunctionType =
        llvm::FunctionType::get(GetCodeGenTypeCache().VoidTy, false);
    isIncompleteFunction = true;
  }

  llvm::Function *llvmFunction =
      llvm::Function::Create(llvmFunctionType, llvm::Function::ExternalLinkage,
                             entry ? llvm::StringRef() : mangledName,
                             GetCodeGenContext().GetLLVMModule());

  return llvmFunction;
}
llvm::Constant *
CodeGenModule::GetOrCreateFunction(llvm::StringRef mangledName,
                                   stone::FunctionDecl *fd, llvm::Type *fnTy,
                                   const EmitFunctionOptions emitFunctionOpts,
                                   llvm::AttributeList extraAttrs) {

  // TODO: Ignoring forDefinition for now -- just creating
  return CreateFunction(mangledName, fd, fnTy, emitFunctionOpts, extraAttrs);
}

llvm::Constant *
CodeGenModule::GetFunctionAddress(stone::FunctionDecl *fd, llvm::Type *fnTy,
                                  const EmitFunctionOptions emitFunctionOpts) {

  // TODO:
  // If there was no specific requested type, just convert it now.
  // if (!functionTy) {
  //   const auto *FD = llvm::cast<FunctionDecl>(GD.getDecl());
  //   functionTy = GetCodeGenTypeResolver().ResolveType(fd->GetType());
  // }

  if (emitFunctionOpts.contains(EmitFunctionFlags::IsForDefinition)) {
  }

  return GetOrCreateFunction("mangledName", fd, fnTy, emitFunctionOpts);
}

void CodeGenModule::SetFunctionLinkage(stone::FunctionDecl *fd,
                                       llvm::Function *fn) {
  fn->setLinkage(GetFunctionLinkage(fd));
}
llvm::GlobalValue::LinkageTypes
CodeGenModule::GetFunctionLinkage(stone::FunctionDecl *fd) {

  // TODO: FOR NOW
  return llvm::GlobalValue::InternalLinkage;
}

llvm::StringRef CodeGenModule::GetMangledNameOfGlobalDecl(Decl *d) {
  const auto *nd = llvm::cast<NameableDecl>(d);
}
