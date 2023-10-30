#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGen.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

CodeGenFunction::CodeGenFunction(CodeGenModule &cgm, llvm::Function *fn)
    : cgm(cgm), fn(fn), builder(cgm.GetCodeGen().GetCodeGenContext(),
                                GetCodeGenModule().GetCodeGenTypeCache()) {}

CodeGenFunction::~CodeGenFunction() {}

/// Create a new basic block with the given name.  The block is not
/// automatically inserted into the function.
llvm::BasicBlock *CodeGenFunction::CreateBasicBlock(const llvm::Twine &name) {
  return llvm::BasicBlock::Create(
      *cgm.GetCodeGen().GetCodeGenContext().GetLLVMContext(), name);
}

void CodeGenFunction::EmitFunction(FunctionDecl *fd) {

  EmitPrologue();

  EmitEpilogue();
}

void CodeGenFunction::EmitPrologue() {
  // assert(!fn &&
  //        "Do not use a CodeGenFunction object for more than one function");
}

void CodeGenFunction::EmitEpilogue() {}
