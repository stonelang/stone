#include "stone/Gen/IRGenBuilder.h"
#include "stone/Gen/IRGenTypeCache.h"

using namespace stone;

void IRGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

IRGenBuilder::IRGenBuilder(IRGen &irGen, const IRGenTypeCache &typeCache)
    : IRGenBuilderBase(irGen.GetLLVMContext()), irGen(irGen),
      typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGen &irGen, const IRGenTypeCache &typeCache,
                           const llvm::ConstantFolder &constFoler,
                           const IRGenBuilderInserter &inserter)
    : IRGenBuilderBase(irGen.GetLLVMContext(), constFoler, inserter),
      irGen(irGen), typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGen &irGen, const IRGenTypeCache &typeCache,
                           llvm::Instruction *instruction)
    : IRGenBuilderBase(instruction), irGen(irGen), typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGen &irGen, const IRGenTypeCache &typeCache,
                           llvm::BasicBlock *basicBlock)
    : IRGenBuilderBase(basicBlock), irGen(irGen), typeCache(typeCache) {}

IRGenBuilder::~IRGenBuilder() {}
