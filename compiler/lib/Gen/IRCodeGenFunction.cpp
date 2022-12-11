#include "stone/Gen/IRCodeGenFunction.h"
#include "stone/Gen/IRCodeGenModule.h"

using namespace stone;

IRCodeGenFunction::IRCodeGenFunction(IRCodeGenModule &codeGenModule,
                                     llvm::Function *curFunction)
    : codeGenModule(codeGenModule), curFunction(curFunction) {}

IRCodeGenFunction::~IRCodeGenFunction() {}