#include "stone/Gen/IRGenFunction.h"
#include "stone/Gen/IRGenModule.h"

using namespace stone;

IRGenFunction::IRGenFunction(IRGenModule &cgm, llvm::Function *llvmFunction)
    : cgm(cgm), llvmFunction(llvmFunction),
      builder(cgm.GetIRGen(), cgm.GetIRGenTypeCache()) {}

IRGenFunction::~IRGenFunction() {}

/// Create a new basic block with the given name.  The block is not
/// automatically inserted into the function.
llvm::BasicBlock *IRGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(cgm.GetIRGen().GetLLVMContext(), name);
}

void IRGenFunction::EmitFunction(FunctionDecl *fd) {

  EmitPrologue();

  EmitEpilogue();
}

void IRGenFunction::EmitPrologue() {

  assert(!llvmFunction &&
         "Do not use a IRGenFunction object for more than one function");

  llvm::BasicBlock *entryBB = CreateBasicBlock("entry");

  assert(llvmFunction->getBasicBlockList().empty() &&
         "prologue already emitted?");

  llvmFunction->getBasicBlockList().push_back(entryBB);

  builder.SetInsertPoint(entryBB);

  // Set up the alloca insertion point.
  // AllocaIP = Builder.IRBuilderBase::CreateAlloca(IGM.Int1Ty,
  ///*array size*/ nullptr,
  //"alloca point");
}

void IRGenFunction::EmitEpilogue() {}
