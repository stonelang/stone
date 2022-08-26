#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"

using namespace stone;

IRCodeGenFunction::IRCodeGenFunction(IRCodeGenModule &irCodeGenModule,
                                     llvm::Function *llvmFun)
    : irCodeGenModule(irCodeGenModule), llvmFunction(llvmFunction) {}

IRCodeGenFunction::~IRCodeGenFunction() {}