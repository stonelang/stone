#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Gen/IRCodeGenABI.h"

#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

IRCodeGenModule::IRCodeGenModule(IRCodeGen &irCodeGen,
                                 llvm::StringRef moduleName,
                                 llvm::StringRef outputFilename)

    : typeCache(*irCodeGen.GetCodeGenContext().GetLLVMContext()),
      irCodeGen(irCodeGen), moduleName(moduleName),
      outputFilename(outputFilename), typeResolver(*this), metadata(*this) {}

IRCodeGenModule::~IRCodeGenModule() {}

llvm::StringRef IRCodeGenModule::GetMangledName(Decl &d) { return ""; }

// llvm::GlobalValue *IRCodeGenModule::GetGlobalValue(llvm::StringRef name) {
//   return GetCodeGenContext().GetLLVMModule()->getNamedValue(Name);
// }
