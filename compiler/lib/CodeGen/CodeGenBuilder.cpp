#include "stone/CodeGen/CodeGenBuilder.h"
#include "stone/CodeGen/CodeGenContext.h"
#include "stone/CodeGen/CodeGenTypeCache.h"

using namespace stone;
using namespace stone::codegen;

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
    : CodeGenBuilderBase(*cgc.GetLLVMContext(), constFoler, inserter), cgc(cgc),
      typeCache(typeCache) {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                               const CodeGenTypeCache &typeCache,
                               llvm::Instruction *instruction)
    : CodeGenBuilderBase(instruction), cgc(cgc), typeCache(typeCache) {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                               const CodeGenTypeCache &typeCache,
                               llvm::BasicBlock *basicBlock)
    : CodeGenBuilderBase(basicBlock), cgc(cgc), typeCache(typeCache) {}

CodeGenBuilder::~CodeGenBuilder() {}
