#include "stone/CodeGen/CodeGenModule.h"
#include "stone/Basic/CodeGenOptions.h"

using namespace stone;


CodeGenModule::CodeGenModule(
    CodeGen &codeGen, SourceFile *sourceFile,
    ModuleNameAndOuptFileName moduleNameAndOuptFileName)
    : codeGen(codeGen), sourceFile(sourceFile),
      moduleNameAndOuptFileName(moduleNameAndOuptFileName),
      codeGenPassMgr(codeGen.GetCodeGenOptions(), nullptr),
      dataLayout(codeGen.GetClangDataLayoutString()),
      clangCodeGen(CreateClangCodeGen()) {

  codeGen.AddCodeGenModule(sourceFile, this);

  GetClangCodeGen().GetModule()->setTargetTriple(
      codeGen.GetEffectiveClangTriple().str());
  GetClangCodeGen().GetModule()->setDataLayout(
      GetDataLayout().getStringRepresentation());

  VoidTy = llvm::Type::getVoidTy(codeGen.GetLLVMContext());

  // Int types
  Int8Ty = llvm::Type::getInt8Ty(codeGen.GetLLVMContext());
  Int16Ty = llvm::Type::getInt16Ty(codeGen.GetLLVMContext());
  Int32Ty = llvm::Type::getInt32Ty(codeGen.GetLLVMContext());
  Int64Ty = llvm::Type::getInt64Ty(codeGen.GetLLVMContext());

  // IntPtr types
  Int8PtrTy = Int8Ty->getPointerTo();
  Int16PtrTy = Int16Ty->getPointerTo();
  Int32PtrTy = Int32Ty->getPointerTo();
  Int64PtrTy = Int16Ty->getPointerTo();
}

std::unique_ptr<clang::CodeGenerator> CodeGenModule::CreateClangCodeGen() {
  return codeGen.GetASTContext().GetClangImporter().CreateCodeGenerator(
      codeGen.GetLLVMContext(), moduleNameAndOuptFileName.first);
}
