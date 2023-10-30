#include "stone/Gen/CodeGenBuilder.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Gen/CodeGenTypeCache.h"

using namespace stone;

void CodeGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                                   const CodeGenTypeCache &typeCache)
    : CodeGenBuilderBase(*cgc.GetLLVMContext()), cgc(cgc),
      typeCache(typeCache) {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                                   const CodeGenTypeCache &typeCache,
                                   const llvm::ConstantFolder &constFoler,
                                   const CodeGenBuilderInserter &inserter)
    : CodeGenBuilderBase(*cgc.GetLLVMContext(), constFoler, inserter),
      cgc(cgc), typeCache(typeCache) {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                                   const CodeGenTypeCache &typeCache,
                                   llvm::Instruction *instruction)
    : CodeGenBuilderBase(instruction), cgc(cgc), typeCache(typeCache) {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                                   const CodeGenTypeCache &typeCache,
                                   llvm::BasicBlock *basicBlock)
    : CodeGenBuilderBase(basicBlock), cgc(cgc), typeCache(typeCache) {}

CodeGenBuilder::~CodeGenBuilder() {}
