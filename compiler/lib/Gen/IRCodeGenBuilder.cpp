#include "stone/Gen/IRCodeGenBuilder.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/IRCodeGenTypeCache.h"

using namespace stone;

void IRCodeGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

IRCodeGenBuilder::IRCodeGenBuilder(CodeGenContext &cgc,
                                   const IRCodeGenTypeCache &typeCache)
    : IRCodeGenBuilderBase(cgc.GetLLVMContext()), cgc(cgc),
      typeCache(typeCache) {}

IRCodeGenBuilder::IRCodeGenBuilder(CodeGenContext &cgc,
                                   const IRCodeGenTypeCache &typeCache,
                                   const llvm::ConstantFolder &constFoler,
                                   const IRCodeGenBuilderInserter &inserter)
    : IRCodeGenBuilderBase(cgc.GetLLVMContext(), constFoler, inserter),
      cgc(cgc), typeCache(typeCache) {}

IRCodeGenBuilder::IRCodeGenBuilder(CodeGenContext &cgc,
                                   const IRCodeGenTypeCache &typeCache,
                                   llvm::Instruction *instruction)
    : IRCodeGenBuilderBase(instruction), cgc(cgc), typeCache(typeCache) {}

IRCodeGenBuilder::IRCodeGenBuilder(CodeGenContext &cgc,
                                   const IRCodeGenTypeCache &typeCache,
                                   llvm::BasicBlock *basicBlock)
    : IRCodeGenBuilderBase(basicBlock), cgc(cgc), typeCache(typeCache) {}

IRCodeGenBuilder::~IRCodeGenBuilder() {}
