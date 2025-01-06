#ifndef STONE_CODEGEN_CODEGENFUNCTION_H
#define STONE_CODEGEN_CODEGENFUNCTION_H

#include "stone/AST/ASTVisitor.h"

#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Function.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Type.h"

#include <memory>

namespace llvm {
class AllocaInst;
class CallSite;
class Constant;
class Function;
} // namespace llvm

namespace stone {

class CodeGenBuilder;
class CodeGenModule;

class CodeGenFunctionInfo {
public:
  /// Whether this is an instance method.
  unsigned InstanceFunction : 1;
};

class CodeGenLoopInfoStack final {
public:
  // void InsertHelper(llvm::Instruction *I) {
  //     // Example: Apply loop unroll metadata
  //     llvm::MDNode *UnrollHint =
  //         llvm::MDNode::get(I->getContext(),
  //                           llvm::MDString::get(I->getContext(),
  //                           "llvm.loop.unroll.enable"));
  //     I->setMetadata("llvm.loop", UnrollHint);
  // }
};

class CodeGenFunction final {

  CodeGenModule &codeGenModule;
  llvm::Function *curFunction = nullptr;
  llvm::BasicBlock *returnBB;

public:
  CodeGenFunction(CodeGenModule &codeGenModule, llvm::Function *curFunction);
  ~CodeGenFunction();

public:
  CodeGenModule &GetCodGenModule() { return codeGenModule; }
  llvm::Function *GetCurFunction() { return curFunction; }

private:
  void EmitPrologue();
  void EmitEpilogue();

public:
  llvm::BasicBlock *CreateBasicBlock(const llvm::Twine &name);
  void EmitFunction(FunctionDecl *FD);

public:
  void InsertHelper(llvm::Instruction *instruction, const llvm::Twine &name,
                    llvm::BasicBlock *basicBlock,
                    llvm::BasicBlock::iterator insertPt) const;
};

} // namespace stone
#endif