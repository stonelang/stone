#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"

using namespace stone;

IRCodeGenFunction::IRCodeGenFunction(IRCodeGenModule &cgm,
                                     llvm::Function *llvmFunction)
    : cgm(cgm), llvmFunction(llvmFunction),
      builder(cgm.GetIRCodeGen(), cgm.GetIRCodeGenTypeCache()) {}

IRCodeGenFunction::~IRCodeGenFunction() {}

/// Create a new basic block with the given name.  The block is not
/// automatically inserted into the function.
llvm::BasicBlock *IRCodeGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(cgm.GetIRCodeGen().GetLLVMContext(), name);
}

void IRCodeGenFunction::EmitFunction(FunctionDecl *fd) {

  EmitPrologue();

  EmitEpilogue();
}

void IRCodeGenFunction::EmitPrologue() {

  assert(!llvmFunction &&
         "Do not use a IRCodeGenFunction object for more than one function");

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

void IRCodeGenFunction::EmitEpilogue() {}
