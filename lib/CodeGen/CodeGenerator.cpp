#include "stone/CodeGen/CodeGenerator.h"
#include "stone/Basic/Ret.h"
#include "stone/Syntax/Module.h"

#include "llvm/IR/Module.h"

using namespace stone;
using namespace stone::codegen;

CodeGenerator::CodeGenerator() {}

// CodeGenModule &GetCodeGenModule();

llvm::Module *CodeGenerator::GetLLVMModule() { return nullptr; }
