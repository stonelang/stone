#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;


void CodeGenFunction::EmitIfStmt(const IfStmt &stmt) {}

Status CodeGenFunction::EmitBasicBlock(llvm::BasicBlock *bb) {
  Status status;
  return status;
}

void CodeGenFunction::EmitBranch(llvm::BasicBlock *target) {
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