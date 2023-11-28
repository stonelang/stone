#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"

using namespace stone;

IRCodeGenFunction::IRCodeGenFunction(IRCodeGenModule &cgm, llvm::Function *fn)
    : cgm(cgm), fn(fn),
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
  // assert(!fn &&
  //        "Do not use a IRCodeGenFunction object for more than one function");
}

void IRCodeGenFunction::EmitEpilogue() {}
