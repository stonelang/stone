#include "stone/Gen/IRFunction.h"
#include "stone/Gen/IRModule.h"

using namespace stone;

IRFunction::IRFunction(IRModule &irModule, llvm::Function *llvmFun)
    : irModule(irModule), llvmFun(llvmFun) {}

IRFunction::~IRFunction() {}