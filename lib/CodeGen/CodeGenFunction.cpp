#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

CodeGenFunction::CodeGenFunction(CodeGenModule &codeGenModule,
                                 llvm::Function *llvmFunction)
    : codeGenModule(codeGenModule), codeGenBuilder(codeGenModule),
      llvmFunction(llvmFunction) {}

llvm::BasicBlock *CodeGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(
      GetCodGenModule().GetCodeGenContext().GetLLVMContext(), name);
}

void CodeGenFunction::EmitFunction(FunctionDecl *FD) {

  EmitPrologue();

  EmitEpilogue();
}

void CodeGenFunction::EmitPrologue() {

  assert(!llvmFunction &&
         "Do not use a CodeGenFunction object for more than one function");

  llvm::BasicBlock *entryBB = CreateBasicBlock("entry");

  GetCodeGenBuilder().SetInsertPoint(entryBB);
}

void CodeGenFunction::EmitEpilogue() {}
