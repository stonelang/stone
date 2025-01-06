#include "stone/CodeGen/CodeGenBuilder.h"
#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"
using namespace stone;

void CodeGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {

  /// TODO?
  llvm::IRBuilderDefaultInserter::InsertHelper(instruction, name, basicBlock,
                                               insertPt);
  if (CGF) {
    CGF->InsertHelper(instruction, name, basicBlock, insertPt);
  }
}

CodeGenBuilder::CodeGenBuilder(CodeGenModule &CGM)
    : CodeGenBuilderBase(CGM.GetCodeGenContext().GetLLVMContext()), CGM(CGM) {}

// CodeGenBuilder::CodeGenBuilder(CodeGenModule &codeGenModule,
//                                const llvm::ConstantFolder &constFoler,
//                                const CodeGenBuilderInserter &inserter)
//     : CodeGenBuilderBase(codeGenModule.GetCodeGenContext().GetLLVMContext(),
//                          constFoler, inserter),
//       codeGenModule(codeGenModule) {}

// CodeGenBuilder::CodeGenBuilder(CodeGenModule &codeGenModule,
//                                llvm::Instruction *instruction)
//     : CodeGenBuilderBase(instruction), codeGenModule(codeGenModule) {}

// CodeGenBuilder::CodeGenBuilder(CodeGenModule &codeGenModule,
//                                llvm::BasicBlock *basicBlock)
//     : CodeGenBuilderBase(basicBlock), codeGenModule(codeGenModule) {}

CodeGenBuilder::~CodeGenBuilder() {}
