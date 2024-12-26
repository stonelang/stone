#ifndef STONE_CODEGEN_CODEGENFUNCTION_H
#define STONE_CODEGEN_CODEGENFUNCTION_H

#include "stone/AST/ASTVisitor.h"
#include "stone/CodeGen/CodeGenBuilder.h"

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

class CodeGenFunction final {

  CodeGenModule &codeGenModule;
  CodeGenBuilder codeGenBuilder;

  llvm::Function *llvmFunction = nullptr;
  llvm::BasicBlock *returnBB;

public:
  CodeGenFunction(CodeGenModule &codeGenModule, llvm::Function *llvmFunction);

public:
  CodeGenModule &GetCodGenModule() { return codeGenModule; }
  llvm::Function *GetLLVMFunction() { return llvmFunction; }
};

} // namespace stone
#endif