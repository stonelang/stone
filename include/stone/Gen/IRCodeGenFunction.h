#ifndef STONE_GEN_IRCODEGENFUNCTION_H
#define STONE_GEN_IRCODEGENFUNCTION_H

#include "stone/Gen/IRCodeGenModule.h"
#include "llvm/IR/Function.h"

#include <memory>

namespace stone {

class IRCodeGenModule;
class IRCodeGenFunction final {

  IRCodeGenModule &irCodeGenModule;
  llvm::Function *llvmFunction = nullptr;

public:
  IRCodeGenFunction(const IRCodeGenFunction &) = delete;
  void operator=(const IRCodeGenFunction &) = delete;

public:
  IRCodeGenFunction(IRCodeGenModule &, llvm::Function *llvmFunction);
  ~IRCodeGenFunction();

public:
};

} // namespace stone

#endif
