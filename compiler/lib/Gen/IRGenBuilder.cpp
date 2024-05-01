#include "stone/Gen/IRGenBuilder.h"
#include "stone/Gen/IRGenTypeCache.h"

using namespace stone;

void IRGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

IRGenBuilder::IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache)
    : IRGenBuilderBase(invocation.GetLLVMContext()), invocation(invocation),
      typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache,
                           const llvm::ConstantFolder &constFoler,
                           const IRGenBuilderInserter &inserter)
    : IRGenBuilderBase(invocation.GetLLVMContext(), constFoler, inserter),
      invocation(invocation), typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache,
                           llvm::Instruction *instruction)
    : IRGenBuilderBase(instruction), invocation(invocation), typeCache(typeCache) {}

IRGenBuilder::IRGenBuilder(IRGenInvocation &invocation, const IRGenTypeCache &typeCache,
                           llvm::BasicBlock *basicBlock)
    : IRGenBuilderBase(basicBlock), invocation(invocation), typeCache(typeCache) {}

IRGenBuilder::~IRGenBuilder() {}
