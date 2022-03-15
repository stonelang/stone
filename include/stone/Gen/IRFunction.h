#ifndef STONE_GEN_IRFUNCTION_H
#define STONE_GEN_IRFUNCTION_H

#include "stone/Gen/IRModule.h"

#include "llvm/IR/Function.h"

#include <memory>

namespace stone {

class IRModule;
class IRFunction final {
  IRModule &irModule;
  llvm::Function *llvmFun = nullptr;

public:
  IRFunction(const IRFunction &) = delete;
  void operator=(const IRFunction &) = delete;

public:
  IRFunction(IRModule &irModule, llvm::Function *llvmFun);
  ~IRFunction();

public:
};

class IRFunctionEmitter final {

public:
  IRFunctionEmitter(const IRFunctionEmitter &) = delete;
  void operator=(const IRFunctionEmitter &) = delete;

public:
  IRFunctionEmitter();
  ~IRFunctionEmitter();

public:
  // void EmitDecl(syn::Decl* decl);
  // void EmitStmt(syn::Stmt* stmt);
};
} // namespace stone

#endif
