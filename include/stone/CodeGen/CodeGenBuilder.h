#ifndef STONE_CODEGEN_CODEGENBUILDER_H
#define STONE_CODEGEN_CODEGENBUILDER_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

namespace stone {

class CodeGenModule;
class CodeGenFunction;

class CodeGenBuilderInserter : public llvm::IRBuilderDefaultInserter {
private:
  CodeGenFunction *CGF = nullptr;

public:
  CodeGenBuilderInserter() = default;
  explicit CodeGenBuilderInserter(CodeGenFunction *CGF) : CGF(CGF) {}

public:
  /// This forwards to CodeGenFunction::InsertHelper.
  void InsertHelper(llvm::Instruction *instruction, const llvm::Twine &name,
                    llvm::BasicBlock *basicBlock,
                    llvm::BasicBlock::iterator insertPt) const override;
};

using CodeGenBuilderBase =
    llvm::IRBuilder<llvm::ConstantFolder, CodeGenBuilderInserter>;

class CodeGenBuilder final : public CodeGenBuilderBase {
  CodeGenModule &CGM;

public:
  using CodeGenBuilderBase::CodeGenBuilderBase;

public:
  CodeGenBuilder(CodeGenModule &CGM);

  // CodeGenBuilder(CodeGenModule &codeGenModule,
  //                const llvm::ConstantFolder &constFoler,
  //                const CodeGenBuilderInserter &inserter);

  // CodeGenBuilder(CodeGenModule &codeGenModule, llvm::Instruction
  // *instruction);

  // CodeGenBuilder(CodeGenModule &codeGenModule, llvm::BasicBlock *basicBlock);

  ~CodeGenBuilder();

public:
  void CreateCall();
};

} // namespace stone
#endif