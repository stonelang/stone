#include "stone/Gen/IRGenFunction.h"
#include "stone/Gen/IRGenInstance.h"

using namespace stone;

IRGenFunction::IRGenFunction(IRGenModule &gm, llvm::Function *llvmFunction)
    : gm(gm), llvmFunction(llvmFunction),
      builder(gm.GetIRGenInstance(), gm.GetIRGenTypeCache()) {}

IRGenFunction::~IRGenFunction() {}

/// Create a new basic block with the given name.  The block is not
/// automatically inserted into the function.
llvm::BasicBlock *IRGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(gm.GetIRGenInstance().GetLLVMContext(), name);
}

void IRGenFunction::EmitFunction(FunctionDecl *fd) {

  EmitPrologue();

  EmitEpilogue();
}

void IRGenFunction::EmitPrologue() {

  assert(!llvmFunction &&
         "Do not use a IRGenFunction object for more than one function");

  llvm::BasicBlock *entryBB = CreateBasicBlock("entry");

  // TODO:
  //  assert(llvmFunction->getBasicBlockList().empty() &&
  //         "prologue already emitted?");

  // llvmFunction->getBasicBlockList().push_back(entryBB);

  builder.SetInsertPoint(entryBB);

  // Set up the alloca insertion point.
  // AllocaIP = Builder.IRBuilderBase::CreateAlloca(IGM.Int1Ty,
  ///*array size*/ nullptr,
  //"alloca point");
}

void IRGenFunction::EmitEpilogue() {}
