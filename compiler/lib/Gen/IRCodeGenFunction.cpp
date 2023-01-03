#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenModule.h"

using namespace stone;

IRCodeGenFunction::IRCodeGenFunction(IRCodeGenModule &cgm, llvm::Function *fn)
    : cgm(cgm), fn(fn), builder(cgm.GetIRCodeGen().GetCodeGenContext(),
                                GetIRCodeGenModule().GetIRCodeGenTypeCache()) {}

IRCodeGenFunction::~IRCodeGenFunction() {}

/// Create a new basic block with the given name.  The block is not
/// automatically inserted into the function.
llvm::BasicBlock *IRCodeGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(
      *cgm.GetIRCodeGen().GetCodeGenContext().GetLLVMContext(), name);
}

Status IRCodeGenFunction::EmitBasicBlock(llvm::BasicBlock *bb) {
  Status status;
  return status;
}

void IRCodeGenFunction::EmitBranch(llvm::BasicBlock *target) {
  // Emit a branch from the current block to the target one if this
  // was a real block.  If this was just a fall-through block after a
  // terminator, don't emit it.
  llvm::BasicBlock *curBB = builder.GetInsertBlock();
  if (!curBB || curBB->getTerminator()) {
    // If there is no insert point or the previous block is already
    // terminated, don't touch it.
  } else {
    // Otherwise, create a fall-through branch.
    builder.CreateBr(target);
  }
  builder.ClearInsertionPoint();
}