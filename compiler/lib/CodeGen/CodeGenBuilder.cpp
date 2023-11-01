#include "stone/CodeGen/CodeGenBuilder.h"
#include "stone/CodeGen/CodeGenContext.h"

using namespace stone;
using namespace stone::codegen;

// TODO: Ok for now -- may move to CodeGenMoulde
CodeGenTypeCache::CodeGenTypeCache(llvm::LLVMContext &llvmContext) {

  VoidTy = llvm::Type::getVoidTy(llvmContext);
  Int8Ty = llvm::Type::getInt8Ty(llvmContext);
  Int16Ty = llvm::Type::getInt16Ty(llvmContext);
  Int32Ty = llvm::Type::getInt32Ty(llvmContext);
  Int32PtrTy = Int32Ty->getPointerTo();
  Int64Ty = llvm::Type::getInt64Ty(llvmContext);
  Int8PtrTy = llvm::Type::getInt8PtrTy(llvmContext);

  // Int8PtrPtrTy = Int8PtrTy->getPointerTo(0);
}

void CodeGenBuilderInserter::InsertHelper(
    llvm::Instruction *instruction, const llvm::Twine &name,
    llvm::BasicBlock *basicBlock, llvm::BasicBlock::iterator insertPt) const {}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                               const CodeGenTypeCache &typeCache)
    : CodeGenBuilderBase(cgc.GetLLVMContext()), cgc(cgc), typeCache(typeCache) {
}

CodeGenBuilder::CodeGenBuilder(CodeGenContext &cgc,
                               const CodeGenTypeCache &typeCache,
                               const llvm::ConstantFolder &constFoler,
                               const CodeGenBuilderInserter &inserter)
    : CodeGenBuilderBase(cgc.GetLLVMContext(), constFoler, inserter), cgc(cgc),
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
