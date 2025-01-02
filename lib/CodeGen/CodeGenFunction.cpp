#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

CodeGenFunction::CodeGenFunction(CodeGenModule &codeGenModule,
                                 llvm::Function *curFunction)
    : codeGenModule(codeGenModule), codeGenBuilder(codeGenModule),
      curFunction(curFunction) {

  EmitPrologue();
}

void CodeGenFunction::EmitPrologue() {

  assert(!curFunction &&
         "Do not use a CodeGenFunction object for more than one function");

  llvm::BasicBlock *entryBB = CreateBasicBlock("entry");
  // assert(curFunction->getBasicBlockList().empty() &&
  //        "prologue already emitted?");

  // curFunction->getBasicBlockList().push_back(entryBB);
  // GetCodeGenBuilder().SetInsertPoint(entryBB);
}

CodeGenFunction::~CodeGenFunction() { EmitEpilogue(); }

llvm::BasicBlock *CodeGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(
      GetCodGenModule().GetCodeGenContext().GetLLVMContext(), name);
}

void CodeGenFunction::EmitFunction(FunctionDecl *FD) {}

void CodeGenFunction::EmitEpilogue() {}
