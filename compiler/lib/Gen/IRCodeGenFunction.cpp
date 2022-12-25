#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"

using namespace stone;

IRCodeGenFunction::IRCodeGenFunction(IRCodeGenModule &cgm,
                                     llvm::Function *curFun)
    : cgm(cgm), curFun(curFun) {}

IRCodeGenFunction::~IRCodeGenFunction() {}