#include "stone/CodeGen/CodeGenFunction.h"
#include "stone/CodeGen/CodeGenModule.h"

using namespace stone;

CodeGenFunction::CodeGenFunction(CodeGenModule &codeGenModule,
                                 llvm::Function *llvmFunction)
    : codeGenModule(codeGenModule), codeGenBuilder(codeGenModule),
      llvmFunction(llvmFunction) {}
