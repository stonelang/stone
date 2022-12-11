#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Gen/CodeGenContext.h"

using namespace stone;

void IRCodeGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

IRCodeGenBuilder::IRCodeGenBuilder(CodeGenContext &cgc)
    : IRCodeGenBuilderBase(cgc.GetLLVMContext()), cgc(cgc) {}