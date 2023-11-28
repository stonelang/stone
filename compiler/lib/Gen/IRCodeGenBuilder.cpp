#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Gen/IRCodeGenTypeCache.h"

using namespace stone;

void IRCodeGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

IRCodeGenBuilder::IRCodeGenBuilder(IRCodeGen &irCodeGen,
                                   const IRCodeGenTypeCache &typeCache)
    : IRCodeGenBuilderBase(irCodeGen.GetLLVMContext()), irCodeGen(irCodeGen),
      typeCache(typeCache) {}

IRCodeGenBuilder::IRCodeGenBuilder(IRCodeGen &irCodeGen,
                                   const IRCodeGenTypeCache &typeCache,
                                   const llvm::ConstantFolder &constFoler,
                                   const IRCodeGenBuilderInserter &inserter)
    : IRCodeGenBuilderBase(irCodeGen.GetLLVMContext(), constFoler, inserter),
      irCodeGen(irCodeGen), typeCache(typeCache) {}

IRCodeGenBuilder::IRCodeGenBuilder(IRCodeGen &irCodeGen,
                                   const IRCodeGenTypeCache &typeCache,
                                   llvm::Instruction *instruction)
    : IRCodeGenBuilderBase(instruction), irCodeGen(irCodeGen),
      typeCache(typeCache) {}

IRCodeGenBuilder::IRCodeGenBuilder(IRCodeGen &irCodeGen,
                                   const IRCodeGenTypeCache &typeCache,
                                   llvm::BasicBlock *basicBlock)
    : IRCodeGenBuilderBase(basicBlock), irCodeGen(irCodeGen),
      typeCache(typeCache) {}

IRCodeGenBuilder::~IRCodeGenBuilder() {}
