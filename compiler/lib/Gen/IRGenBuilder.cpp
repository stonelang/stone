#include "stone/Gen/IRGenBuilder.h"
#include "stone/Gen/IRGenTypeCache.h"

using namespace stone;

void IRGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

IRGenBuilder::IRGenBuilder(IRGenInstance &instance,
                           const IRGenTypeCache &typeCache)
    : IRGenBuilderBase(instance.GetLLVMContext()), instance(instance),
      typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGenInstance &instance,
                           const IRGenTypeCache &typeCache,
                           const llvm::ConstantFolder &constFoler,
                           const IRGenBuilderInserter &inserter)
    : IRGenBuilderBase(instance.GetLLVMContext(), constFoler, inserter),
      instance(instance), typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGenInstance &instance,
                           const IRGenTypeCache &typeCache,
                           llvm::Instruction *instruction)
    : IRGenBuilderBase(instruction), instance(instance),
      typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGenInstance &instance,
                           const IRGenTypeCache &typeCache,
                           llvm::BasicBlock *basicBlock)
    : IRGenBuilderBase(basicBlock), instance(instance),
      typeCache(typeCache) {}

IRGenBuilder::~IRGenBuilder() {}
