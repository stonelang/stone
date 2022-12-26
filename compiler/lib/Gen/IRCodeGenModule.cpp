#include "stone/Gen/IRCodeGenModule.h"
#include "stone/Gen/IRCodeGen.h"
#include "stone/Syntax/Decl.h"

using namespace stone;
using namespace stone::syn;

IRCodeGenModule::IRCodeGenModule(IRCodeGen &irCodeGen,
                                 llvm::StringRef moduleName,
                                 llvm::StringRef outputFilename)

    : irCodeGen(irCodeGen), moduleName(moduleName),
      outputFilename(outputFilename) {}

IRCodeGenModule::~IRCodeGenModule() {}
