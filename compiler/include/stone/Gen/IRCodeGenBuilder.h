#ifndef STONE_GEN_IRCODEGENBUILDER_H
#define STONE_GEN_IRCODEGENBUILDER_H

#include "stone/Basic/LLVM.h"
#include "stone/Gen/CodeGenContext.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

#include <memory>

namespace llvm {
class raw_pwrite_stream;
class GlobalVariable;
class MemoryBuffer;
class Module;
class TargetOptions;
class TargetMachine;
} // namespace llvm

namespace stone {

class IRCodeGenFunction;

class IRCodeGenBuilderInserter : public llvm::IRBuilderDefaultInserter {
private:
  IRCodeGenFunction *cgf = nullptr;

public:
  IRCodeGenBuilderInserter() = default;
  explicit IRCodeGenBuilderInserter(IRCodeGenFunction *cgf) : cgf(cgf) {}

  /// This forwards to CodeGenFunction::InsertHelper.
  void InsertHelper(llvm::Instruction *instruction, const llvm::Twine &name,
                    llvm::BasicBlock *basicBlock,
                    llvm::BasicBlock::iterator insertPt) const override;
};

using IRCodeGenBuilderBase =
    llvm::IRBuilder<llvm::ConstantFolder, IRCodeGenBuilderInserter>;

class IRCodeGenBuilder : public IRCodeGenBuilderBase {
  CodeGenContext &cgc;

public:
  IRCodeGenBuilder(CodeGenContext &cgc);
  ~IRCodeGenBuilder();
};
} // namespace stone

#endif